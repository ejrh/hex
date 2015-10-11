#ifndef UPDATER_H
#define UPDATER_H

#include "hex/messaging/receiver.h"


class Updater: public MessageReceiver {
public:
    Updater(int id);
    virtual ~Updater();

    void subscribe(MessageReceiver *receiver);
    void unsubscribe(MessageReceiver *receiver);
    virtual void receive(boost::shared_ptr<Message> msg);

private:
    void send_update_to_subscribers(boost::shared_ptr<Message> update);

private:
    int id;
    int next_message_id;

    std::vector<MessageReceiver *> subscribers;
};


#endif
