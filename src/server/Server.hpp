#pragma once

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>

class Session : public std::enable_shared_from_this<Session> {
   public:
    using TcpSocket = boost::asio::ip::tcp;
    using NetworkSize = uint64_t;

    Session(TcpSocket::socket rtSocket) : mtSocket(std::move(rtSocket)) {}

    void Start();

   private:
    static constexpr size_t MAX_LENGTH = 64 * 1024;
    unsigned int mdCounter = 0;
    TcpSocket::socket mtSocket;
    char maData[MAX_LENGTH];
    size_t mdFileSize;
    NetworkSize mdExpectedSize;
    std::string mtFileName;
    std::ofstream mtOutputFile;

    void DoReadFileData();
    void DoRead();
    void CreateFile();
};

class Server {
   public:
    using TcpSocket = boost::asio::ip::tcp;

    Server(boost::asio::io_context& rtIoContext, short rdPort,
           std::string const& rtWorkDirectory);

   private:
    TcpSocket::acceptor mtAcceptor;
    std::string mtWorkDirectory;

    void DoAccept();
    void CreateWorkDirectory();
};
