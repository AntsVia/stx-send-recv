#include "Session.hpp"
#include "ServerMessageHandler.hpp"
#include "MessageHandlerI.hpp"

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>

void Session::Start() {
    SetState(tools::make_unique<InitSessionState<Session>>());
}

void Session::SetState(std::unique_ptr<SessionState<Session>> rpNewState) {
    mpState = std::move(rpNewState);
    mpState->Start(*this); // kick off state logic
}

void Session::DoReadFileData() {
    auto self(shared_from_this());
    mtSocket.async_read_some(
        boost::asio::buffer(&mdExpectedSize, sizeof(mdExpectedSize)),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (ec) return;
            if (mtFileName.empty()) {
                CreateFile();
            } else {
                std::cout << "Expected size: " << mdExpectedSize << "\n";
                DoRead();
            }
        });
}

void Session::DoRead() {
    auto self(shared_from_this());
    boost::asio::async_read(mtSocket,
               boost::asio::buffer(&mdExpectedSize, sizeof(mdExpectedSize)),
               boost::asio::bind_executor(mtStrand,
                                [this, self] (boost::system::error_code ec, std::size_t /*length*/) {
                                std::cout << "Message size: " << mdExpectedSize <<"\n";
                                if (!ec && MAX_LENGTH > mdExpectedSize) {
                                    std::cout << "Reading message\n";
                                    DoReadMessage();
                                } else {
                                    std::cout << "Read error: " << ec.message() << "\n";
                                }
                                }));
}

void Session::DoReadMessage() {
    auto self(shared_from_this());
    boost::asio::async_read(mtSocket,
               boost::asio::buffer(maData, mdExpectedSize),
               boost::asio::bind_executor(mtStrand,
               [this, self](boost::system::error_code ec, std::size_t length) {
                                if (!ec || length == mdExpectedSize) {
                                    /* deserialize data */
                                    for(unsigned i = 0; i < mdExpectedSize; i++) {
                                          std::cout << maData[i];
                                          }
                                          std::cout << "\n";
                                    std::string buf(maData, length);
                                    std::cout << "Read: " << buf << "\n";
                                    if (this->mpState) {
                                        std::cout << "Start OnRead\n";
                                        this->mpState->OnRead(*this, buf);
                                          }
                                } else {
                                    std::cout << "Read len: " << length << "\n";
                                    std::cout << "Read error: " << ec.message() << "\n";
                                }
               }));
}

void Session::DoWrite(const std::string& raData) {
    auto self(shared_from_this());
    uint32_t uMsgSize = raData.size();
    std::cout << "Write: " << raData << "size: " << uMsgSize << "\n";
    std::vector<boost::asio::const_buffer> aBufs {
        boost::asio::buffer(&uMsgSize, sizeof(uMsgSize)),
        boost::asio::buffer(raData),
    };
    boost::asio::async_write(
        mtSocket,
        aBufs,
        boost::asio::bind_executor(mtStrand,
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (ec) {
                return;
            }
            DoRead();
        }));
}

void Session::Close() {
    // Cancel all outstanding asynchronous operations.
    mtOutputFile.close();
}

// void Session::DoRead() {
//     static int counter = 1;
//     std::cout << "Do read " << counter++ << std::endl;
//     auto self(shared_from_this());
//     async_read(mtSocket, boost::asio::buffer(maData, MAX_LENGTH),
//                [this, self](boost::system::error_code ec, std::size_t length) {
//                    if (ec && ec != boost::asio::error::eof) {
//                        assert(length <= mdExpectedSize);
//                        std::cout << "error code: " << ec << "\n"
//                                  << "Expected size: " << mdExpectedSize
//                                  << " received: " << length << "\n";
//                        mtOutputFile.close();
//                        return;
//                    }
//                    mtOutputFile.write(maData, length);
//                    mdExpectedSize -= length;
//                    if (mtOutputFile.good() && mdExpectedSize == 0 &&
//                        (!ec || ec == boost::asio::error::eof)) {
//                        std::cout << "File content received successfully."
//                                  << std::endl;
//                        mtOutputFile.close();
//                        return;
//                    }
//                    std::cout << "Filesize expected: " << mdExpectedSize << "\n";
//                    DoRead();
//                });
// }

void Session::CreateFile() {
    auto self(shared_from_this());
    std::cout << __LINE__ << " Creating file\n";
    boost::asio::async_read(
        mtSocket, boost::asio::buffer(maData, mdExpectedSize),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (ec) return;

            mtFileName = std::string(maData, length);
            std::cout << __LINE__ << " Filename: " << mtFileName << "\n";
            mtOutputFile.open(mtFileName, std::ios_base::binary);
            if (!mtOutputFile) {
                std::cerr << __LINE__ << ": Failed to create: " << mtFileName;
                return;
            }

            DoReadFileData();
        });
}

