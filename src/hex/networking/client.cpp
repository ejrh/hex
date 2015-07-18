#include "common.h"

#include "hex/basics/error.h"
#include "hex/networking/networking.h"

Client::Client(boost::asio::io_service& io_service):
    socket(io_service), resolver(io_service) {

}

Client::~Client() {
}

void Client::connect(std::string server, int port) {
    tcp::resolver::query query(server);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    boost::asio::async_connect(socket, endpoint_iterator, boost::bind(&Client::handle_connect, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
}

void Client::handle_connect(const boost::system::error_code& error, tcp::resolver::iterator iterator) {
    if (error) {
        trace("Error in handle_connect: %s\n", error.message().c_str());
        return;
    }
}
