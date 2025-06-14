#include "Server.hpp"
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <string>

void Session::Start() { DoReadFileData(); }

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
  static int counter = 1;
  std::cout << "Do read " << counter++ << std::endl;
  auto self(shared_from_this());
  async_read(mtSocket, boost::asio::buffer(maData, MAX_LENGTH),
             [this, self](boost::system::error_code ec, std::size_t length) {
             if (ec && ec != boost::asio::error::eof) {
             assert(length <= mdExpectedSize);
             std::cout << "error code: " << ec << "\n"
             << "Expected size: " << mdExpectedSize
             << " received: " << length << "\n";
             mtOutputFile.close();
             return;
             }
             mtOutputFile.write(maData, length);
             mdExpectedSize -= length;
             if (mtOutputFile.good() && mdExpectedSize == 0 &&
             (!ec || ec == boost::asio::error::eof)) {
             std::cout << "File content received successfully."
             << std::endl;
             mtOutputFile.close();
             return;
             }
             std::cout << "Filesize expected: " << mdExpectedSize << "\n";
             DoRead();
             });
}

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

Server::Server(boost::asio::io_context& rtIoContext, short rdPort,
       std::string const& rtWorkDirectory)
  : mtAcceptor(rtIoContext, TcpSocket::endpoint(TcpSocket::v4(), rdPort)),
  mtWorkDirectory(rtWorkDirectory)
{
  CreateWorkDirectory();
  DoAccept();
}

void Server::DoAccept() {
  mtAcceptor.async_accept(
    [this](boost::system::error_code ec, TcpSocket::socket socket) {
      if (!ec) {
        std::make_shared<Session>(std::move(socket))->Start();
      }

      DoAccept();
    });
}

void Server::CreateWorkDirectory() {
  using namespace boost::filesystem;
  auto tCurrentPath = path(mtWorkDirectory);
  if (!exists(tCurrentPath) && !create_directory(tCurrentPath))
    std::cerr << "Coudn't create working directory: " << mtWorkDirectory;
  current_path(tCurrentPath);
}
