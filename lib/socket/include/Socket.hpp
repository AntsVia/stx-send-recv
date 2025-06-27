#pragma once
#include <boost/asio.hpp>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

class Socket {
   public:
    using TcpSocket = boost::asio::ip::tcp;
    using NetworkSize = uint32_t;

    Socket(boost::asio::io_context& rtIoContext, std::string const& rsAddr,
           std::string const& rsPort);
    Socket(TcpSocket::socket&& rSocket);

    void DoConnect(std::function<void()> onStart);

    template <std::size_t N>
    void DoRead(std::array<char, N>& data,
                std::function<void(const std::string&)> onRead);

    void DoWrite(const std::string& data, std::function<void()> onWrite);

    void Close();

    TcpSocket::socket& getSocket();
    boost::asio::strand<boost::asio::io_context::executor_type>& getStrand();

   private:
    TcpSocket::socket mSocket;
    NetworkSize muExpectedSize;
    boost::asio::strand<boost::asio::any_io_executor> mStrand;
    TcpSocket::resolver::results_type mtEndpoints;

    template <std::size_t N>
    void DoReadMessage(NetworkSize& expectedSize, std::array<char, N>& data,
                       std::function<void(const std::string&)> onRead);
};

template <std::size_t N>
void Socket::DoRead(std::array<char, N>& data,
                    std::function<void(const std::string&)> onRead) {
    boost::asio::async_read(
        mSocket, boost::asio::buffer(&muExpectedSize, sizeof(muExpectedSize)),
        boost::asio::bind_executor(
            mStrand, [this, &data, onRead](boost::system::error_code ec,
                                           std::size_t length) {
                std::cout << "Message size: " << muExpectedSize << "\n";
                if (!ec) {
                    if (!onRead) {
                        std::cout << __LINE__ << "Failed on Read callback\n";
                        return;
                    }
                    DoReadMessage(muExpectedSize, data, onRead);
                } else {
                    std::cout << "len: " << length << "\n";
                    std::cout << "Read error: " << ec.message() << "\n";
                }
            }));
}

template <std::size_t N>
void Socket::DoReadMessage(NetworkSize& expectedSize, std::array<char, N>& data,
                           std::function<void(const std::string&)> onRead) {
    boost::asio::async_read(
        mSocket, boost::asio::buffer(data.data(), expectedSize),
        boost::asio::bind_executor(
            mStrand, [this, &data, &expectedSize, onRead](
                         boost::system::error_code ec, std::size_t length) {
                if (!ec || length == expectedSize) {
                    std::string input(data.begin(), length);
                    std::cout << "Read: " << input << "\n";
                    if (!onRead) {
                        std::cout << __LINE__ << "Failed on Read callback\n";
                        return;
                    }
                    onRead(input);
                } else if (ec == boost::asio::error::eof) {
                    std::cout << "Transition success\n";
                } else {
                    std::cout << "Read len: " << length << "\n";
                    std::cout << "Read error: " << ec.message() << "\n";
                }
            }));
}
