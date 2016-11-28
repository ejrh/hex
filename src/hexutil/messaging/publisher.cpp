#include "common.h"

#include "hexutil/messaging/message.h"
#include "hexutil/messaging/publisher.h"
#include "hexutil/messaging/receiver.h"


Publisher::Publisher(): id(0), next_message_id(1) {
}

Publisher::Publisher(int id): id(id), next_message_id(1) {
}

Publisher::~Publisher() {
}

void Publisher::receive(Message *msg) {
    msg->origin = id;
    msg->id = next_message_id++;
    send_update_to_subscribers(msg);
}

void Publisher::subscribe(MessageReceiver *subscriber) {
    subscribers.push_back(subscriber);
}

void Publisher::unsubscribe(MessageReceiver *subscriber) {
    auto iter = std::find(subscribers.begin(), subscribers.end(), subscriber);
    subscribers.erase(iter);
}

void Publisher::send_update_to_subscribers(Message *update) {
    for (auto iter = subscribers.begin(); iter != subscribers.end(); iter++) {
        MessageReceiver *subscriber = *iter;
        subscriber->receive(update);
    }
}
