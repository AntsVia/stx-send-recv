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
    using NetworkSize = uint32_t;

   FileTransferClient(const FileTransferClient&) = delete;
   FileTransferClient& operator=(const FileTransferClient&) = delete;
   FileTransferClient(FileTransferClient&&) = delete;
   FileTransferClient& operator=(FileTransferClient&&) = delete;

    FileTransferClient(boost::asio::io_context& rtIoContext,
                       const TcpSocket::resolver::results_type& reEndpoints,
                       std::string const& rsPath);
    ~FileTransferClient();

    void DoRead();
    void DoWrite(const std::string& raData);
    void SetState(std::unique_ptr<SessionState<FileTransferClient>> rpNewState);
    void Close();

   private:
    static constexpr size_t BUF_LEN = 65 * 1024;
    TcpSocket::socket mtSocket;
    boost::asio::streambuf mtInputBuffer;
    std::array<char, BUF_LEN> maData;
    std::string mtFilePath;
    std::ifstream mtSourceFile;
    boost::asio::strand<boost::asio::any_io_executor> mtStrand;
    std::unique_ptr<SessionState<FileTransferClient>> mpState;
    NetworkSize mdExpectedSize;

    void DoConnect(const TcpSocket::resolver::results_type& endpoints);
    void DoReadMessage();
    void OpenFile(std::string const& rsPath);
    void SendPacket();
};
