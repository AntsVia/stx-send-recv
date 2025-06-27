#include "Socket.hpp"

#include <iostream>

Socket::Socket(boost::asio::io_context& rtIoContext, std::string const& rsAddr,
               std::string const& rsPort)
    : mSocket(rtIoContext), mStrand(mSocket.get_executor()) {
    TcpSocket::resolver resolver(rtIoContext);
    mtEndpoints = resolver.resolve(rsAddr, rsPort);
}

Socket::Socket(TcpSocket::socket&& rSocket)
    : mSocket(std::move(rSocket)), mStrand(mSocket.get_executor()) {}

void Socket::DoConnect(std::function<void()> onStart) {
    boost::asio::async_connect(
        mSocket, mtEndpoints,
        [onStart](boost::system::error_code ec, TcpSocket::endpoint) {
            if (!ec) {
                if (onStart) onStart();
            }
        });
}

// template<std::size_t N>
// void Socket::DoRead(
//     // uint32_t& expectedSize,
//     std::array<char, N>& data,
//     std::function<void(const std::string&)> onRead)
// {
//     boost::asio::async_read(mSocket,
//         boost::asio::buffer(&muExpectedSize, sizeof(muExpectedSize)),
//         boost::asio::bind_executor(mStrand,
//             [this, &data, onRead](boost::system::error_code ec, std::size_t
//             length) {
//                 std::cout << "Message size: " << muExpectedSize << "\n";
//                 if (!ec) {
//                     if (!onRead) {
//                         std::cout << __LINE__ << "Failed on Read callback\n";
//                         return;
//                     }
//                     DoReadMessage(muExpectedSize, data, onRead);
//                 } else {
//                     std::cout << "len: " << length << "\n";
//                     std::cout << "Read error: " << ec.message() << "\n";
//                 }
//             }
//         )
//     );
// }
//
// template<std::size_t N>
// void Socket::DoReadMessage(
//     NetworkSize& expectedSize,
//     std::array<char, N>& data,
//     std::function<void(const std::string&)> onRead)
// {
//     data.resize(expectedSize);
//     boost::asio::async_read(mSocket,
//         boost::asio::buffer(data.data(), expectedSize),
//         boost::asio::bind_executor(mStrand,
//             [this, &data, &expectedSize, onRead](boost::system::error_code
//             ec, std::size_t length) {
//                 if (!ec || length == expectedSize) {
//                     std::string input(data.begin(), data.end());
//                     std::cout << "Read: " << input << "\n";
//                     if (!onRead) {
//                         std::cout << __LINE__ << "Failed on Read callback\n";
//                         return;
//                     }
//                     onRead(input);
//                 } else if (ec == boost::asio::error::eof) {
//                     std::cout << "Transition success\n";
//                 } else {
//                     std::cout << "Read len: " << length << "\n";
//                     std::cout << "Read error: " << ec.message() << "\n";
//                 }
//             }
//         )
//     );
// }

void Socket::DoWrite(const std::string& data, std::function<void()> onWrite) {
    NetworkSize uMsgSize = data.size();
    std::cout << "Write: " << data << "\n";
    std::vector<boost::asio::const_buffer> bufs{
        boost::asio::buffer(&uMsgSize, sizeof(uMsgSize)),
        boost::asio::buffer(data),
    };
    boost::asio::async_write(
        mSocket, bufs,
        boost::asio::bind_executor(
            mStrand, [this, onWrite](boost::system::error_code ec,
                                     std::size_t /*length*/) {
                if (ec) {
                    return;
                }
                if (!onWrite) {
                    std::cout << __LINE__ << "Failed on Write callback\n";
                    return;
                }
                onWrite();
            }));
}

void Socket::Close() { mSocket.close(); }

// Socket::TcpSocket::socket& Socket::getSocket() { return mSocket; }
//
// boost::asio::strand<boost::asio::io_context::executor_type>&
// Socket::getStrand() {
//     return mStrand;
// }
