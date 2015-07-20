#ifndef NETWORKING_H
#define NETWORKING_H

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"

using boost::asio::ip::tcp;

class Connection: public boost::enable_shared_from_this<Connection> {
public:
    typedef boost::shared_ptr<Connection> pointer;

    static pointer create(boost::asio::io_service& io_service, MessageReceiver *receiver) {
        return pointer(new Connection(io_service, receiver));
    }

    void start();

private:
    Connection(boost::asio::io_service& io_service, MessageReceiver *receiver): receiver(receiver), socket(io_service) { }

    void send_message(boost::shared_ptr<Message> msg);
    void continue_reading();
    void continue_writing();
    void handle_write(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);

    MessageReceiver *receiver;
    tcp::socket socket;
    boost::asio::streambuf buffer;
    std::deque<boost::shared_ptr<Message> > send_queue;
    std::string out_message;

    friend class Server;
    friend class Client;
};

class Server {
public:
    Server(int port, MessageReceiver *receiver);
    ~Server();
    void start();
    void stop();

private:
    void run_thread();
    void start_accept();
    void handle_accept(Connection::pointer new_connection, const boost::system::error_code& error);

private:
    MessageReceiver *receiver;
    boost::asio::io_service io_service;
    boost::thread server_thread;
    tcp::acceptor acceptor;
    bool shutdown_requested;
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
