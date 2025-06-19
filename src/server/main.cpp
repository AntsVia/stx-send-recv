#include <boost/asio.hpp>
#include <iostream>

#include "Server.hpp"

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "Usage: stx-server <port> <path/to/file>\n";
            return 1;
        }

        Server s(std::atoi(argv[1]), 8, argv[2]);

        s.Run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
