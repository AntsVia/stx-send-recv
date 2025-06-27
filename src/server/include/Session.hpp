#pragma once

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <memory>
#include <string>

#include "MessageHandlerI.hpp"
#include "Socket.hpp"

class Session : public std::enable_shared_from_this<Session> {
   public:
    using TcpSocket = boost::asio::ip::tcp;
    using NetworkSize = uint32_t;

    Session(TcpSocket::socket rtSocket) : mSocket(std::move(rtSocket)) {}
    ~Session() = default;

    void Start();
    void DoRead();
    void DoWrite(const std::string& raData);
    void SetState(std::unique_ptr<SessionState<Session>> rpNewState);
    void Close();

   private:
    static constexpr size_t BUF_LEN = 65 * 1024;
    unsigned int mdCounter = 0;
    Socket mSocket;
    std::array<char, BUF_LEN> maData;
    size_t mdFileSize;
    std::string mtFileName;
    std::ofstream mtOutputFile;
    std::unique_ptr<SessionState<Session>> mpState;

    // void DoReadFileData();
    // void CreateFile();
};
