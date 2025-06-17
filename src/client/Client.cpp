#include "Client.hpp"
#include "ClientMessageHandler.hpp"
#include "MessageHandlerI.hpp"

#include <array>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>

#define BOOST_FILESYSTEM_NO_DEPRECATED

using namespace boost::filesystem;

FileTransferClient::FileTransferClient(
    boost::asio::io_context& io_context,
    const TcpSocket::resolver::results_type& endpoints,
    std::string const& rsPath)
    : mtSocket(io_context), mtInputBuffer(BUF_LEN), mtFilePath(rsPath) {
    DoConnect(endpoints);
}

void FileTransferClient::DoConnect(
    const TcpSocket::resolver::results_type& endpoints) {
    boost::asio::async_connect(
        mtSocket, endpoints,
        [this](boost::system::error_code ec, TcpSocket::endpoint) {
            if (!ec) {
                SetState(tools::make_unique<InitSessionState<FileTransferClient>>());
                OpenFile(mtFilePath);
            }
        });
}

void FileTransferClient::SetState(std::unique_ptr<SessionState<FileTransferClient>> rpNewState) {
    mpState = std::move(rpNewState);
    mpState->Start(*this); // kick off state logic
}

void FileTransferClient::DoRead() {
    boost::asio::async_read_until(mtSocket, mtDataInput, "\n",
                                [this] (boost::system::error_code ec, std::size_t /*length*/) {
                                if (!ec) {
                                    /* deserialize data */
                                    std::istream buf(&mtDataInput);
                                    std::string input;
                                    buf >> input;
                                    if (this->mpState)
                                        this->mpState->OnRead(*this, input);
                                } else {
                                    std::cout << "Read error: " << ec.message() << "\n";
                                    SetState(tools::make_unique<FinishSessionState<FileTransferClient>>());
                                }
                                });
}

void FileTransferClient::DoWrite(const std::string& rpData) {
    boost::asio::async_write(
        mtSocket,
        boost::asio::buffer(&rpData, sizeof(rpData)),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
            if (ec) {
                return;
            }
        });
}

void FileTransferClient::OpenFile(std::string const& rsPath) {
    mtSourceFile.open(rsPath, std::ios_base::binary | std::ios_base::ate);
    if (mtSourceFile.fail())
        throw std::fstream::failure("Failed while opening file " + rsPath);

    mtSourceFile.seekg(0, mtSourceFile.end);
    boost::filesystem::path file_path(rsPath);
    NetworkSize fileSize = mtSourceFile.tellg();
    mtSourceFile.seekg(0, mtSourceFile.beg);
    NetworkSize fileNameSize = file_path.filename().string().size();

    std::vector<boost::asio::const_buffer> vPayload{
        boost::asio::buffer(&fileNameSize, sizeof(fileSize)),
        boost::asio::buffer(file_path.filename().string()),
        boost::asio::buffer(&fileSize, sizeof(fileSize)),
    };

    boost::asio::async_write(
        mtSocket, vPayload,
        [this](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec && mtSourceFile.good()) {
                std::cout << "Starting send packets\n";
                SendPacket();
            } else {
                Close();
            }
        });
}

void FileTransferClient::SendPacket() {
    static int counter = 1;
    mtSourceFile.read(maData.data(), BUF_LEN);
    if (mtSourceFile.fail() && !mtSourceFile.eof()) {
        std::cerr << __LINE__ << "Failed reading!\n";
        Close();
        return;
    }
    std::cout << __LINE__ << "times: " << counter++
              << "Data read: " << mtSourceFile.gcount() << " pointer at>>"
              << mtSourceFile.tellg() << "\n";
    boost::asio::async_write(
        mtSocket,
        boost::asio::buffer(maData, static_cast<size_t>(mtSourceFile.gcount())),
        [this](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec && mtSourceFile.good()) {
                SendPacket();
            } else {
                std::cout << "closed\n";
                auto status = (mtSourceFile.fail() && !mtSourceFile.eof())
                                  ? "failed"
                                  : "good";
                std::cout << "err value: " << ec << " read status: " << status
                          << "\n";
                Close();
            }
        });
}

void FileTransferClient::Close() {
    // Cancel all outstanding asynchronous operations.
    mtSourceFile.close();
    mtSocket.close();
}
