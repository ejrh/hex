#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/messaging/message.h"
#include "hexutil/networking/networking.h"
#include "hexutil/messaging/builtin_messages.h"


Server::Server(int port, MessageReceiver *receiver):
        port(port), receiver(receiver), io_service(), acceptor(io_service), shutdown_requested(false),
        next_connection_id(1), game_id(0), last_message_id(0), last_dropped_id(0), max_backlog_size(1000) {
}

Server::~Server() {
}

void Server::start() {
    server_thread = boost::thread(&Server::run_thread, this);
}

void Server::stop() {
    shutdown_requested = true;
    io_service.stop();
    server_thread.join();
}

void Server::receive(boost::shared_ptr<Message> msg) {
    io_service.post(boost::bind(&Server::broadcast, this, msg));
}

void Server::broadcast(boost::shared_ptr<Message> msg) {
    if (msg->id <= last_message_id) {
        BOOST_LOG_TRIVIAL(warning) << "Message with id " << msg->id << " has already been sent";
    }

    message_backlog[msg->id] = msg;
    for (auto iter = connections.begin(); iter != connections.end(); iter++) {
        iter->second->send_message(msg);
    }
    last_message_id = msg->id;
    while (message_backlog.size() > max_backlog_size) {
        last_dropped_id = message_backlog.begin()->second->id;
        message_backlog.erase(message_backlog.begin());
    }
}

void Server::receive_from_network(boost::shared_ptr<Message> msg) {
    Connection::pointer source_connection = connections[msg->origin];

    if (msg->type == StreamReplay) {
        auto replay = boost::dynamic_pointer_cast<StreamReplayMessage>(msg);
        int client_game_id = replay->data1;
        int msg_id = replay->data1;
        bool full_state = false;

        if (client_game_id != game_id) {
            BOOST_LOG_TRIVIAL(warning) << boost::format("Expected game id %d but got %d") % game_id % client_game_id;
            full_state = true;
        }

        if (msg_id < last_dropped_id) {
            BOOST_LOG_TRIVIAL(warning) << boost::format("Message replay requested from %d but last dropped was %d") % msg_id % last_dropped_id;
            full_state = true;
        }

        if (msg_id > last_message_id) {
            BOOST_LOG_TRIVIAL(warning) << boost::format("Message replay requested from %d but latest message is only %d") % msg_id % message_backlog.end()->second->id;
            full_state = true;
        }

        if (full_state) {
            BOOST_LOG_TRIVIAL(debug) << "Sending full state";
            //TODO
        } else {
            BOOST_LOG_TRIVIAL(debug) << "Replaying up to " << message_backlog.size() << " messages";
            for (auto iter = message_backlog.begin(); iter != message_backlog.end(); iter++) {
                if (iter->first > msg_id) {
                    source_connection->send_message(iter->second);
                }
            }
        }

        source_connection->send_message(create_message(StreamState, game_id, last_message_id));
    }

    receiver->receive(msg);
}

void Server::run_thread() {
    tcp::endpoint endpoint(tcp::v4(), port);
    acceptor.open(endpoint.protocol());
    acceptor.set_option(boost::asio::socket_base::reuse_address(true));
    acceptor.bind(endpoint);
    acceptor.listen(5);
    start_accept();
    io_service.run();
}

void Server::start_accept() {
    Connection::pointer new_connection = Connection::create(acceptor.get_io_service(), this);
    acceptor.async_accept(new_connection->socket, boost::bind(&Server::handle_accept, this, new_connection, boost::asio::placeholders::error));
}

void Server::handle_accept(Connection::pointer new_connection, const boost::system::error_code& error) {
    if (!error) {
        new_connection->start();
        new_connection->send_message(create_message(StreamOpen, std::string("Hex Server 0.1")));
        new_connection->id = next_connection_id++;
        connections[new_connection->id] = new_connection;
    }
    if (!shutdown_requested)
        start_accept();
}
