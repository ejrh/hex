#ifndef MESSAGE_LOADER_H
#define MESSAGE_LOADER_H

#include "hex/basics/objmap.h"
#include "hex/graphics/graphics.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"

class MessageLoader: public MessageReceiver {
public:
    MessageLoader() { }

    void receive(boost::shared_ptr<Message> msg);

    void load(const std::string& filename);
    void include(const std::string& filename, bool skip_missing = false);

    const std::string& get_current_file() const;

    virtual void handle_message(boost::shared_ptr<Message> msg) = 0;

private:
    std::vector<std::string> current_files;
};

class ReceiverMessageLoader: public MessageLoader {
public:
    ReceiverMessageLoader(MessageReceiver& receiver): receiver(receiver) { }

    void handle_message(boost::shared_ptr<Message> msg) {
        receiver.receive(msg);
    };

private:
    MessageReceiver& receiver;
};

#endif
