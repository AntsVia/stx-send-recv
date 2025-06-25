#pragma once

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <memory>
#include <string>

#include "MessageHandlerI.hpp"

class Session : public std::enable_shared_from_this<Session> {
   public:
    using TcpSocket = boost::asio::ip::tcp;
    using NetworkSize = uint32_t;

    Session(TcpSocket::socket rtSocket)
        : mtSocket(std::move(rtSocket)),
          mtStrand(boost::asio::make_strand(mtSocket.get_executor())) {}
    ~Session() = default;

    void Start();
    void DoRead();
    void DoWrite(const std::string& raData);
    void SetState(std::unique_ptr<SessionState<Session>> rpNewState);
    void Close();

   private:
    static constexpr size_t MAX_LENGTH = 65 * 1024;
    unsigned int mdCounter = 0;
    TcpSocket::socket mtSocket;
    char maData[MAX_LENGTH];
    size_t mdFileSize;
    NetworkSize mdExpectedSize;
    std::string mtFileName;
    std::ofstream mtOutputFile;
    std::unique_ptr<SessionState<Session>> mpState;
    boost::asio::streambuf mtDataInput;
    boost::asio::strand<boost::asio::any_io_executor> mtStrand;

    void DoReadFileData();
    void DoReadMessage();
    void CreateFile();
};
