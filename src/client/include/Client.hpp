#pragma once

#include <array>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <memory>
#include <string>

#include "MessageHandlerI.hpp"
#include "Socket.hpp"

class FileTransferClient {
   public:
    using TcpSocket = boost::asio::ip::tcp;
    using NetworkSize = uint32_t;

    FileTransferClient(const FileTransferClient&) = delete;
    FileTransferClient& operator=(const FileTransferClient&) = delete;
    FileTransferClient(FileTransferClient&&) = delete;
    FileTransferClient& operator=(FileTransferClient&&) = delete;

    FileTransferClient(boost::asio::io_context& rtIoContext,
                       std::string const& rsAddr, std::string const& rsPort,
                       std::string const& rsPath);
    ~FileTransferClient();

    void DoRead();
    void DoWrite(const std::string& raData);
    void SetState(std::unique_ptr<SessionState<FileTransferClient>> rpNewState);
    void Close();

   private:
    static constexpr size_t BUF_LEN = 65 * 1024;
    Socket mSocket;
    std::array<char, BUF_LEN> maData;
    std::string mtFilePath;
    std::ifstream mtSourceFile;
    std::unique_ptr<SessionState<FileTransferClient>> mpState;

    void OpenFile(std::string const& rsPath);
    void SendPacket();
};
