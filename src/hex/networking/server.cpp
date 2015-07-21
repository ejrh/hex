#include "common.h"

#include "hex/basics/error.h"
#include "hex/messaging/message.h"
#include "hex/game/game.h"
#include "hex/game/game_serialisation.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_updater.h"
#include "hex/networking/networking.h"

Server::Server(int port, MessageReceiver *receiver):
        receiver(receiver), io_service(), acceptor(io_service, tcp::endpoint(tcp::v4(), port)), shutdown_requested(false) {
    start_accept();
}

Server::~Server() {

}

void Server::start() {
    server_thread = boost::thread(&Server::run_thread, this);
    trace("started");
}

void Server::stop() {
    shutdown_requested = true;
    io_service.stop();
    server_thread.join();
    trace("stopped");
}

void Server::run_thread() {
    trace("run_thread");
    io_service.run();
    trace("/run_thread");
}

void Server::start_accept() {
    Connection::pointer new_connection = Connection::create(acceptor.get_io_service(), receiver);
    acceptor.async_accept(new_connection->socket, boost::bind(&Server::handle_accept, this, new_connection, boost::asio::placeholders::error));
}

void Server::handle_accept(Connection::pointer new_connection, const boost::system::error_code& error) {
    if (!error) {
        new_connection->start();
        new_connection->send_message(boost::make_shared<WrapperMessage<std::string> >(StreamOpen, std::string("Hex Server 0.1")));
    }
    if (!shutdown_requested)
        start_accept();
}
