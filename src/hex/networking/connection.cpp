#include "common.h"

#include "hex/basics/error.h"
#include "hex/networking/networking.h"
#include "hex/messaging/serialiser.h"
#include "hex/messaging/message.h"

void Connection::start() {
    static std::string message("hello\n");

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
        trace("Error in handle_write: %s\n", error.message().c_str());
        return;
    }

    send_queue.pop_front();
    if (!send_queue.empty())
        continue_writing();
}

void Connection::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (error) {
        trace("Error in handle_read: %s\n", error.message().c_str());
        return;
    }

    std::istream stream(&buffer);
    Deserialiser reader(stream);
    Message *msg_ptr;
    reader >> msg_ptr;
    if (msg_ptr == NULL) {
        trace("Message not read");
        return;
    }
    boost::shared_ptr<Message> msg(msg_ptr);

    receiver->receive(msg);

    continue_reading();
}
