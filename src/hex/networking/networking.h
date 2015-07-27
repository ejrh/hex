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

class NetworkInterface: public MessageReceiver {
public:
    virtual void receive_from_network(boost::shared_ptr<Message> msg) = 0;
};

class Connection: public boost::enable_shared_from_this<Connection> {
public:
    typedef boost::shared_ptr<Connection> pointer;

    static pointer create(boost::asio::io_service& io_service, NetworkInterface *iface) {
        return pointer(new Connection(io_service, iface));
    }

    Connection(boost::asio::io_service& io_service, NetworkInterface *iface);
    ~Connection();

private:
    void start();
    void send_message(boost::shared_ptr<Message> msg);
    void continue_reading();
    void continue_writing();
    void handle_write(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);

    int id;
    NetworkInterface *iface;
    tcp::socket socket;
    boost::asio::streambuf buffer;
    std::deque<boost::shared_ptr<Message> > send_queue;
    std::string out_message;

    friend class Server;
    friend class Client;
};

class Server: public NetworkInterface {
public:
    Server(int port, MessageReceiver *receiver);
    virtual ~Server();
    void start();
    void stop();
    virtual void receive(boost::shared_ptr<Message> msg);

private:
    void broadcast(boost::shared_ptr<Message> msg);
    void receive_from_network(boost::shared_ptr<Message> msg);
    void run_thread();
    void start_accept();
    void handle_accept(Connection::pointer new_connection, const boost::system::error_code& error);

private:
    int port;
    MessageReceiver *receiver;
    boost::asio::io_service io_service;
    boost::thread server_thread;
    tcp::acceptor acceptor;
    bool shutdown_requested;

    int next_connection_id;
    std::map<int, Connection::pointer> connections;

    int game_id;
    int last_message_id;
    int last_dropped_id;
    unsigned int max_backlog_size;
    std::map<int, boost::shared_ptr<Message> > message_backlog;
};

class Client: public NetworkInterface {
public:
    Client(MessageReceiver *receiver);
    virtual ~Client();
    void connect(std::string server);
    void disconnect();
    virtual void receive(boost::shared_ptr<Message> msg);

private:
    void receive_from_network(boost::shared_ptr<Message> msg);
    void run_thread();
    void handle_connect(const boost::system::error_code& error, tcp::resolver::iterator iterator);

private:
    MessageReceiver *receiver;
    boost::asio::io_service io_service;
    boost::thread client_thread;
    tcp::resolver resolver;
    bool shutdown_requested;
    Connection::pointer connection;
    int game_id;
    int last_received_id;
};

#endif
