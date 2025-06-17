#include <boost/asio.hpp>
#include <iostream>

#include "Client.hpp"

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "Usage: stx-client <address> <port> <filePath>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints =
            resolver.resolve(argv[1], argv[2]);

        FileTransferClient c(io_context, endpoints, argv[3]);

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
