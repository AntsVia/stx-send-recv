#pragma once

#include "MessageHandlerI.hpp"

#include <array>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <memory>
#include <string>

class FileTransferClient {
   public:
    using TcpSocket = boost::asio::ip::tcp;
    using NetworkSize = uint64_t;

    FileTransferClient(boost::asio::io_context& io_context,
                       const TcpSocket::resolver::results_type& endpoints,
                       std::string const& rsPath);
    ~FileTransferClient() = default;

    void DoRead();
    void DoWrite(const std::string& rpData);
    void SetState(std::unique_ptr<SessionState<FileTransferClient>> rpNewState);
    void Close();

   private:
    static constexpr size_t BUF_LEN = 64 * 1024;
    TcpSocket::socket mtSocket;
    boost::asio::streambuf mtInputBuffer;
    std::array<char, BUF_LEN> maData;
    std::string mtFilePath;
    std::ifstream mtSourceFile;
    boost::asio::streambuf mtDataInput;
    std::unique_ptr<SessionState<FileTransferClient>> mpState;

    void DoConnect(const TcpSocket::resolver::results_type& endpoints);
    void OpenFile(std::string const& rsPath);
    void SendPacket();
};
