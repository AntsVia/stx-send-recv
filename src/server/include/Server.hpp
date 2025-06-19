#pragma once

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <string>

class Server {
   public:
    using TcpSocket = boost::asio::ip::tcp;

   Server(const Server&) = delete;

   Server& operator=(const Server&) = delete;

    // explicit Server(boost::asio::io_context& rtIoContext, short rdPort,
    //        std::string const& rtWorkDirectory);

    explicit Server(short rdPort, std::size_t rdThreadPoolSize,
                    std::string const& rtWorkDirectory);
    ~Server() = default;
    
    void Run();

   private:
   // The number of threads that will call io_context::run().
   std::size_t mdThreadPoolSize;

   // The io_context used to perform asynchronous operations.
   boost::asio::io_context mtIoContext;

    // Acceptor used to listen for incoming connections.
    TcpSocket::acceptor mtAcceptor;
    std::string mtWorkDirectory;

    void DoAccept();
    void CreateWorkDirectory(std::string const& rtWorkDirectory);
};
