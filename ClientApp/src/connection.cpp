// src/connection.cpp

#include "connection.h"
#include <iostream>
#include <fstream>

using boost::asio::ip::tcp;
namespace asio = boost::asio;

// void start_server(asio::io_context &io_context, short port)
// {
//     tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v6(), port)); // Use IPv6
//     std::cout << "Server listening on port " << port << " (IPv6)" << std::endl;

//     // tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
//     // std::cout << "Server listening on port " << port << std::endl;

//     while (true)
//     {
//         tcp::socket socket(io_context);
//         acceptor.accept(socket);
//         std::string message = "Hello from Server!\n";
//         std::cout << "Server sending: " << message;
//         boost::system::error_code ignored_error;
//         boost::asio::write(socket, asio::buffer(message), ignored_error);
//     }
// }

void start_server(asio::io_context &io_context, short port, const std::string &file_path)
{
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v6(), port));
    std::cout << "Server listening on port " << port << " (IPv6)" << std::endl;

    while (true)
    {
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        // Open the file
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " << file_path << std::endl;
            continue;
        }

        // Get the file size
        file.seekg(0, std::ios::end);
        auto file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        // Send the file size first
        std::string file_size_str = std::to_string(file_size) + "\n";
        boost::asio::write(socket, asio::buffer(file_size_str));

        // Read and send the file in chunks
        std::vector<char> buffer(1024);
        while (file)
        {
            file.read(buffer.data(), buffer.size());
            boost::asio::write(socket, asio::buffer(buffer.data(), file.gcount()));
        }
    }
}

// void start_client(asio::io_context &io_context, const std::string &server, short port)
// {
//     tcp::socket socket(io_context);
//     socket.connect(tcp::endpoint(asio::ip::make_address_v6(server), port)); // Use IPv6

//     // tcp::socket socket(io_context);
//     // socket.connect(tcp::endpoint(asio::ip::address::from_string(server), port));

//     std::vector<char> buffer(128);
//     boost::system::error_code error;
//     size_t len = socket.read_some(asio::buffer(buffer), error);

//     if (!error)
//     {
//         std::cout.write(buffer.data(), len);
//     }
// }

void start_client(asio::io_context &io_context, const std::string &server, short port, const std::string &output_file)
{
    tcp::socket socket(io_context);
    socket.connect(tcp::endpoint(asio::ip::make_address_v6(server), port));

    // Read the file size first
    std::string file_size_str;
    boost::asio::streambuf buf;
    std::istream input_stream(&buf);
    boost::asio::read_until(socket, buf, "\n");
    std::getline(input_stream, file_size_str);
    std::size_t file_size = std::stoll(file_size_str);

    // Open a file to write the incoming data
    std::ofstream output(output_file, std::ios::binary);
    if (!output.is_open())
    {
        std::cerr << "Failed to open file for writing: " << output_file << std::endl;
        return;
    }

    // Read and write the file in chunks
    std::vector<char> buffer(1024);
    std::size_t received = 0;
    while (received < file_size)
    {
        size_t len = socket.read_some(asio::buffer(buffer));
        output.write(buffer.data(), len);
        received += len;
    }
}
