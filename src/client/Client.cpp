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
    boost::asio::io_context& rtIoContext,
    const TcpSocket::resolver::results_type& reEndpoints,
    std::string const& rsPath)
    : mtSocket(boost::asio::make_strand(rtIoContext)), mtInputBuffer(BUF_LEN), mtFilePath(rsPath), mtStrand(boost::asio::make_strand(rtIoContext)) {
    DoConnect(reEndpoints);
}

FileTransferClient::~FileTransferClient() {
    Close();
}

void FileTransferClient::DoConnect(
    const TcpSocket::resolver::results_type& endpoints) {
    boost::asio::async_connect(
        mtSocket, endpoints,
        [this](boost::system::error_code ec, TcpSocket::endpoint) {
            if (!ec) {
                SetState(tools::make_unique<InitSessionState<FileTransferClient>>());
                // OpenFile(mtFilePath);
            }
        });
}

void FileTransferClient::SetState(std::unique_ptr<SessionState<FileTransferClient>> rpNewState) {
    mpState = std::move(rpNewState);
    mpState->Start(*this);
}

void FileTransferClient::DoRead() {
    boost::asio::async_read(mtSocket,
               boost::asio::buffer(&mdExpectedSize, sizeof(mdExpectedSize)),
                            boost::asio::bind_executor(mtStrand,
                                [this] (boost::system::error_code ec, std::size_t /*length*/) {
                                std::cout << "Message size: " << mdExpectedSize <<"\n";
                                if (!ec && BUF_LEN > mdExpectedSize) {
                                    DoReadMessage();
                                } else {
                                    std::cout << "Read error: " << ec.message() << "\n";
                                }
                                }));
}

void FileTransferClient::DoReadMessage() {
    boost::asio::async_read(mtSocket,
               boost::asio::buffer(maData.data(), mdExpectedSize),
                            boost::asio::bind_executor(mtStrand,
               [this](boost::system::error_code ec, std::size_t length) {
                                if (!ec || length == mdExpectedSize) {
                                    /* deserialize data */
                                    std::string input(maData.data(), length);
                                    std::cout << "Read: " << input << "\n";
                                    if (this->mpState)
                                        this->mpState->OnRead(*this, input);
                                } else if (ec == boost::asio::error::eof) {
                                    std::cout << "Transition success\n";
                                } else {
                                    std::cout << "Read len: " << length << "\n";
                                    std::cout << "Read error: " << ec.message() << "\n";
                                }
               }));
}

void FileTransferClient::DoWrite(const std::string& raData) {
    uint32_t uMsgSize = raData.size();
    std::cout << "Write: " << raData << "\n";
    std::vector<boost::asio::const_buffer> aBufs {
        boost::asio::buffer(&uMsgSize, sizeof(uMsgSize)),
        boost::asio::buffer(raData),
    };
    boost::asio::async_write(
        mtSocket,
        aBufs,
        boost::asio::bind_executor(mtStrand,
        [this](boost::system::error_code ec, std::size_t /*length*/) {
            if (ec) {
                return;
            }
            if (this->mpState) {
                 this->mpState->OnWrite(*this);
            }
        }));
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
        boost::asio::buffer(&fileNameSize, sizeof(fileNameSize)),
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
    mtSourceFile.close();
    mtSocket.close();
}
