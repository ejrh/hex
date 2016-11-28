#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/messaging/message.h"
#include "hexutil/networking/networking.h"
#include "hexutil/messaging/builtin_messages.h"


Client::Client(MessageReceiver *receiver):
        receiver(receiver), io_service(), resolver(io_service), game_id(0), last_received_id(0) {
}

Client::~Client() {
}

void Client::connect(std::string server) {
    std::string host;
    std::string port("9999");

    std::vector<std::string> strs;
    boost::split(strs, server, boost::is_any_of(":"));
    host = strs[0];

    if (strs.size() > 1) {
        port = strs[1];
    }

    tcp::resolver::query query(host, port);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    connection = Connection::create(io_service, this);
    connection->id = 0;
    boost::asio::async_connect(connection->socket, endpoint_iterator, boost::bind(&Client::handle_connect, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));

    client_thread = boost::thread(&Client::run_thread, this);
}

void Client::disconnect() {
    shutdown_requested = true;
    io_service.stop();
    client_thread.join();
}

void Client::receive(Message *msg) {
    io_service.post(boost::bind(&Connection::send_message, connection, msg->shared_from_this()));
}

void Client::receive_from_network(Message *msg) {
    last_received_id = msg->id;

    if (msg->type == StreamState) {
        auto state = dynamic_cast<StreamStateMessage *>(msg);
        game_id = state->data1;
        last_received_id = state->data2;
        BOOST_LOG_TRIVIAL(debug) << "Received state for game " << game_id << " up to message " << last_received_id;
    }

    receiver->receive(msg);
}

void Client::run_thread() {
    io_service.run();
}

void Client::handle_connect(const boost::system::error_code& error, tcp::resolver::iterator iterator) {
    if (error) {
        BOOST_LOG_TRIVIAL(error) << "Error in handle_connect: " << error.message();
        return;
    }

    connection->start();
    connection->send_message(create_message(StreamOpen, std::string("Hex Client 0.1")));
    connection->send_message(create_message(StreamReplay, game_id, last_received_id));
}
