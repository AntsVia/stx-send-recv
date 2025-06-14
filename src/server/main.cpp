#include <boost/asio.hpp>
#include <iostream>
#include "Server.hpp"

int main(int argc, char* argv[]) {
  try {
    if (argc != 3) {
      std::cerr << "Usage: async_tcp_echo_server <port> <path/to/file\n";
      return 1;
    }

    boost::asio::io_context io_context;

    Server s(io_context, std::atoi(argv[1]), argv[2]);

    io_context.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
