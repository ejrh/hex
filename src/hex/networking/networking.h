#ifndef NETWORKING_H
#define NETWORKING_H

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Connection: public boost::enable_shared_from_this<Connection> {
public:
    typedef boost::shared_ptr<Connection> pointer;

    static pointer create(boost::asio::io_service& io_service) {
        return pointer(new Connection(io_service));
    }

    void start();

private:
    Connection(boost::asio::io_service& io_service): socket(io_service) { }

    void handle_write(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);

    tcp::socket socket;
    boost::asio::streambuf buffer;

    friend class Server;
    friend class Client;
};

class Server {
public:
    Server(int port, boost::asio::io_service& io_service);
    ~Server();

private:
    void start_accept();
    void handle_accept(Connection::pointer new_connection, const boost::system::error_code& error);

private:
    tcp::acceptor acceptor;
};

class Client {
public:
    Client(boost::asio::io_service& io_service);
    ~Client();

    void connect(std::string server, int port);
    void handle_connect(const boost::system::error_code& error, tcp::resolver::iterator iterator);

private:
    tcp::socket socket;
    tcp::resolver resolver;
};

#endif
