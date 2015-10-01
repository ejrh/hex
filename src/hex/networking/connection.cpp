#include "common.h"

#include "hex/basics/error.h"
#include "hex/messaging/serialiser.h"
#include "hex/messaging/message.h"
#include "hex/networking/networking.h"


Connection::Connection(boost::asio::io_service& io_service, NetworkInterface *iface): iface(iface), socket(io_service) {
}

Connection::~Connection() {
}

void Connection::start() {
    continue_reading();
}

void Connection::send_message(boost::shared_ptr<Message> msg) {
    bool send_in_progress = !send_queue.empty();
    send_queue.push_back(msg);
    if (!send_in_progress) {
        continue_writing();
    }
}

void Connection::continue_reading() {
    boost::asio::async_read_until(socket, buffer, '\n',
        boost::bind(&Connection::handle_read, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void Connection::continue_writing() {
    boost::shared_ptr<Message> msg = send_queue.front();

    std::ostringstream stream;
    Serialiser writer(stream);
    writer << msg.get();
    out_message = stream.str();

    boost::asio::async_write(socket, boost::asio::buffer(out_message),
        boost::bind(&Connection::handle_write, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void Connection::handle_write(const boost::system::error_code& error, size_t bytes_transferred) {
    if (error) {
        BOOST_LOG_TRIVIAL(error) << "Error in handle_write: " << error.message();
        return;
    }

    BOOST_LOG_TRIVIAL(debug) << boost::format("Connection %d send %s") % id % out_message;

    send_queue.pop_front();
    if (!send_queue.empty())
        continue_writing();
}

void Connection::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (error) {
        BOOST_LOG_TRIVIAL(error) << "Error in handle_read: " << error.message();
        return;
    }

    std::istream stream(&buffer);
    Deserialiser reader(stream);
    Message *msg_ptr;
    try {
        reader >> msg_ptr;
        if (msg_ptr == NULL) {
            BOOST_LOG_TRIVIAL(warning) << "Message not read";
            return;
        }
    } catch (Error &ex) {
        BOOST_LOG_TRIVIAL(error) << "Failed reading message from network with: " << ex.what();
        return;
    }

    boost::shared_ptr<Message> msg(msg_ptr);
    msg->origin = id;

    {
        std::ostringstream ss;
        Serialiser writer(ss);
        writer << msg.get();
        std::string in_message(ss.str());
        BOOST_LOG_TRIVIAL(debug) << boost::format("Connection %d recv %s") % id % in_message;
    }

    iface->receive_from_network(msg);

    continue_reading();
}
