#include "Session.hpp"

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include "MessageHandlerI.hpp"
#include "ServerMessageHandler.hpp"

void Session::Start() {
    SetState(tools::make_unique<InitSessionState<Session>>());
}

void Session::SetState(std::unique_ptr<SessionState<Session>> rpNewState) {
    mpState = std::move(rpNewState);
    mpState->Start(*this);  // kick off state logic
}

// void Session::DoReadFileData() {
//     auto self(shared_from_this());
//     mtSocket.async_read_some(
//         boost::asio::buffer(&mdExpectedSize, sizeof(mdExpectedSize)),
//         [this, self](boost::system::error_code ec, std::size_t /*length*/) {
//             if (ec) return;
//             if (mtFileName.empty()) {
//                 CreateFile();
//             } else {
//                 std::cout << "Expected size: " << mdExpectedSize << "\n";
//                 DoRead();
//             }
//         });
// }

void Session::DoRead() {
    auto self(shared_from_this());
    mSocket.DoRead(maData, [this, self](const std::string& buf) {
        if (this->mpState) this->mpState->OnRead(*this, buf);
    });
}

void Session::DoWrite(const std::string& raData) {
    auto self(shared_from_this());
    mSocket.DoWrite(raData, [this, self]() {
        if (this->mpState) this->mpState->OnWrite(*this);
    });
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
//                [this, self](boost::system::error_code ec, std::size_t length)
//                {
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
//                    std::cout << "Filesize expected: " << mdExpectedSize <<
//                    "\n"; DoRead();
//                });
// }

// void Session::CreateFile() {
//     auto self(shared_from_this());
//     std::cout << __LINE__ << " Creating file\n";
//     boost::asio::async_read(
//         mtSocket, boost::asio::buffer(maData.data(), mdExpectedSize),
//         [this, self](boost::system::error_code ec, std::size_t length) {
//             if (ec) return;
//
//             mtFileName = std::string(maData.data(), length);
//             std::cout << __LINE__ << " Filename: " << mtFileName << "\n";
//             mtOutputFile.open(mtFileName, std::ios_base::binary);
//             if (!mtOutputFile) {
//                 std::cerr << __LINE__ << ": Failed to create: " <<
//                 mtFileName; return;
//             }
//
//             DoReadFileData();
//         });
// }
