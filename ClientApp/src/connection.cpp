#include "connection.h"
#include <iostream>

using boost::asio::ip::tcp;
namespace asio = boost::asio;

void start_server(asio::io_context &io_context, short port)
{
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v6(), port)); // Use IPv6
    std::cout << "Server listening on port " << port << " (IPv6)" << std::endl;

    // tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
    // std::cout << "Server listening on port " << port << std::endl;

    while (true)
    {
        tcp::socket socket(io_context);
        acceptor.accept(socket);
        std::string message = "Hello from Server!\n";
        std::cout << "Server sending: " << message;
        boost::system::error_code ignored_error;
        boost::asio::write(socket, asio::buffer(message), ignored_error);
    }
}

void start_client(asio::io_context &io_context, const std::string &server, short port)
{
    tcp::socket socket(io_context);
    socket.connect(tcp::endpoint(asio::ip::make_address_v6(server), port)); // Use IPv6

    // tcp::socket socket(io_context);
    // socket.connect(tcp::endpoint(asio::ip::address::from_string(server), port));

    std::vector<char> buffer(128);
    boost::system::error_code error;
    size_t len = socket.read_some(asio::buffer(buffer), error);

    if (!error)
    {
        std::cout.write(buffer.data(), len);
    }
}
