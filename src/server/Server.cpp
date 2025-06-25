#include "Server.hpp"
#include "Session.hpp"

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

Server::Server(short rdPort, std::size_t rdThreadPoolSize,
               std::string const& rtWorkDirectory)
     : mdThreadPoolSize(rdThreadPoolSize),
       mtAcceptor(mtIoContext, TcpSocket::endpoint(TcpSocket::v4(), rdPort))
{
    CreateWorkDirectory(rtWorkDirectory);
    DoAccept();
}

void Server::Run()
{
  std::vector<std::thread> aThreads;
  for (std::size_t i = 0; i < mdThreadPoolSize; ++i)
    aThreads.emplace_back([this]{ mtIoContext.run(); });

  for (std::size_t i = 0; i < aThreads.size(); ++i)
    aThreads[i].join();
}

void Server::DoAccept() {
    mtAcceptor.async_accept(boost::asio::make_strand(mtIoContext),
        [this](boost::system::error_code ec, TcpSocket::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket))->Start();
            }

            DoAccept();
        });
}

void Server::CreateWorkDirectory(std::string const& rtWorkDirectory) {
    using namespace boost::filesystem;
    auto tCurrentPath = path(rtWorkDirectory);
    if (!exists(tCurrentPath) && !create_directory(tCurrentPath))
        std::cerr << "Coudn't create working directory: " << rtWorkDirectory;
    current_path(tCurrentPath);
}
