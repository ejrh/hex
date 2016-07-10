#include "common.h"

#include "hex/messaging/message.h"
#include "hex/messaging/publisher.h"
#include "hex/messaging/receiver.h"


Publisher::Publisher(): id(0), next_message_id(1) {
}

Publisher::Publisher(int id): id(id), next_message_id(1) {
}

Publisher::~Publisher() {
}

void Publisher::receive(boost::shared_ptr<Message> msg) {
    msg->origin = id;
    msg->id = next_message_id++;
    send_update_to_subscribers(msg);
}

void Publisher::subscribe(MessageReceiver *subscriber) {
    subscribers.push_back(subscriber);
}

void Publisher::unsubscribe(MessageReceiver *subscriber) {
    std::vector<MessageReceiver *>::iterator iter = std::find(subscribers.begin(), subscribers.end(), subscriber);
    subscribers.erase(iter);
}

void Publisher::send_update_to_subscribers(boost::shared_ptr<Message> update) {
    for (std::vector<MessageReceiver *>::iterator iter = subscribers.begin(); iter != subscribers.end(); iter++) {
        MessageReceiver *subscriber = *iter;
        subscriber->receive(update);
    }
}
