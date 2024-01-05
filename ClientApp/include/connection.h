#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/asio.hpp>

using boost::asio::ip::tcp;
namespace asio = boost::asio;

// void start_server(asio::io_context &io_context, short port);
void start_server(asio::io_context &io_context, short port, const std::string &file_path);

// void start_client(asio::io_context &io_context, const std::string &server, short port);
void start_client(asio::io_context &io_context, const std::string &server, short port, const std::string &output_file);

#endif // CONNECTION_H
