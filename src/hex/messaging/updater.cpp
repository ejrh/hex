#include "common.h"

#include "hex/messaging/serialiser.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"
#include "hex/messaging/updater.h"


Updater::Updater(int id): id(id), next_message_id(1) {
}

Updater::~Updater() {
}

void Updater::receive(boost::shared_ptr<Message> msg) {
    msg->origin = id;
    msg->id = next_message_id++;
    send_update_to_subscribers(msg);
}

void Updater::subscribe(MessageReceiver *subscriber) {
    subscribers.push_back(subscriber);
}

void Updater::send_update_to_subscribers(boost::shared_ptr<Message> update) {
    for (std::vector<MessageReceiver *>::iterator iter = subscribers.begin(); iter != subscribers.end(); iter++) {
        MessageReceiver *subscriber = *iter;
        subscriber->receive(update);
    }
}
