#include "common.h"

#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"
#include "hexutil/messaging/queue.h"


MessageQueue::MessageQueue(unsigned int capacity):
        capacity(capacity) {
}

MessageQueue::~MessageQueue() {
}

void MessageQueue::receive(Message *msg) {
    boost::lock_guard<boost::mutex> guard(mtx);
    queue.push_back(msg->shared_from_this());
    if (queue.size() > capacity) {
        BOOST_LOG_TRIVIAL(warning) << "Queue size exceeds capacity";
    }
}

boost::shared_ptr<Message> MessageQueue::fetch_message() {
    boost::lock_guard<boost::mutex> guard(mtx);
    boost::shared_ptr<Message> result;
    if (queue.empty())
        return result;
    result = queue[0];
    queue.erase(queue.begin());
    return result;
}

int MessageQueue::fetch_messages(std::vector<boost::shared_ptr<Message> >& results) {
    boost::lock_guard<boost::mutex> guard(mtx);
    std::swap(queue, results);
    int num = results.size();
    return num;
}

int MessageQueue::flush(MessageReceiver *receiver) {
    boost::lock_guard<boost::mutex> guard(mtx);
    int num = queue.size();
    for (unsigned int i = 0; i < queue.size(); i++) {
        receiver->receive(queue[i].get());
    }
    queue.clear();
    return num;
}
