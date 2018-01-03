#ifndef PUBLISHER_H
#define PUBLISHER_H

#include "hexutil/messaging/receiver.h"


namespace hex {

class Publisher: public MessageReceiver {
public:
    Publisher();
    Publisher(int id);
    virtual ~Publisher();

    void subscribe(MessageReceiver *receiver);
    void unsubscribe(MessageReceiver *receiver);

    virtual void receive(Message *msg);
    using MessageReceiver::receive;

private:
    void send_update_to_subscribers(Message *update);

private:
    int id;
    int next_message_id;

    std::vector<MessageReceiver *> subscribers;
};

};

#endif
