#include "common.h"

#include "hex/basics/error.h"
#include "hex/networking/networking.h"

void Connection::start() {
    static std::string message("hello\n");

    boost::asio::async_write(socket, boost::asio::buffer(message),
        boost::bind(&Connection::handle_write, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void Connection::handle_write(const boost::system::error_code& error, size_t bytes_transferred) {
    if (error) {
        trace("Error in handle_write: %s\n", error.message().c_str());
        return;
    }

    boost::asio::async_read_until(socket, buffer, '\n',
        boost::bind(&Connection::handle_read, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void Connection::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (error) {
        trace("Error in handle_read: %s\n", error.message().c_str());
        return;
    }

    boost::asio::streambuf::const_buffers_type bufs = buffer.data();
    std::string line(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + bytes_transferred);
    trace("Recevied {%s}\n", line.c_str());
    buffer.consume(bytes_transferred);

    static std::string message("ok\n");

    boost::asio::async_write(socket, boost::asio::buffer(message),
        boost::bind(&Connection::handle_write, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

Server::Server(int port, boost::asio::io_service& io_service):
        acceptor(io_service, tcp::endpoint(tcp::v4(), port)) {
    start_accept();
}

Server::~Server() {

}

void Server::start_accept() {
    Connection::pointer new_connection = Connection::create(acceptor.get_io_service());
    acceptor.async_accept(new_connection->socket, boost::bind(&Server::handle_accept, this, new_connection, boost::asio::placeholders::error));
}

void Server::handle_accept(Connection::pointer new_connection, const boost::system::error_code& error) {
    if (!error) {
        new_connection->start();
    }
    start_accept();
}
