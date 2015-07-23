#include "common.h"

#include "hex/basics/error.h"
#include "hex/messaging/message.h"
#include "hex/game/game.h"
#include "hex/game/game_serialisation.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_updater.h"
#include "hex/networking/networking.h"

Client::Client(MessageReceiver *receiver):
    receiver(receiver), io_service(), resolver(io_service) {
        trace("Client");
}

Client::~Client() {
    trace("/Client");
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

    connection = Connection::create(io_service, receiver);
    boost::asio::async_connect(connection->socket, endpoint_iterator, boost::bind(&Client::handle_connect, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));

    client_thread = boost::thread(&Client::run_thread, this);
    //run_thread();
}

void Client::disconnect() {
}

void Client::receive(boost::shared_ptr<Message> msg) {
    io_service.post(boost::bind(&Connection::send_message, connection, msg));
}

void Client::run_thread() {
    trace("Client run_thread");
    io_service.run();
    trace("Client /run_thread");
}

void Client::handle_connect(const boost::system::error_code& error, tcp::resolver::iterator iterator) {
    if (error) {
        trace("Error in handle_connect: %s\n", error.message().c_str());
        return;
    }

    connection->start();
    connection->send_message(boost::make_shared<WrapperMessage<std::string> >(StreamOpen, std::string("Hex Client 0.1")));
}
