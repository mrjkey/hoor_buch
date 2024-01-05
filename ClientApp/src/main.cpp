#include <iostream>
#include "math_utils.h"
#include "connection.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
namespace asio = boost::asio;

int main(int argc, char **argv)
{
    // Check the number of parameters
    // if (argc < 3)
    // {
    //     std::cerr << "Usage: " << argv[0] << " <port> <peer_address> <peer_port>" << std::endl;
    //     return 1;
    // }

    // int port = std::stoi(argv[1]);
    // std::string peer_address = argv[2];
    // int peer_port = std::stoi(argv[3]);

    // std::cout << "Starting Client on port " << port << std::endl;
    // std::cout << "Connecting to peer at " << peer_address << ":" << peer_port << std::endl;

    std::cout << "Starting on port " << std::endl;

    try
    {
        if (argc != 4)
        {
            std::cerr << "Usage: ClientApp <server> <port> <mode: server|client>" << std::endl;
            return 1;
        }

        asio::io_context io_context;
        short port = std::stoi(argv[2]);

        if (std::string(argv[3]) == "server")
        {
            start_server(io_context, port, "../test.txt");
        }
        else
        {
            start_client(io_context, argv[1], port, "test.txt");
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}