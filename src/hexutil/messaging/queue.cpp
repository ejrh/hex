#include "common.h"

#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"
#include "hexutil/messaging/queue.h"


MessageQueue::MessageQueue(unsigned int capacity):
        capacity(capacity) {
}

MessageQueue::~MessageQueue() {
}

void MessageQueue::receive(boost::shared_ptr<Message> msg) {
    boost::lock_guard<boost::mutex> guard(mtx);
    queue.push_back(msg);
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
    for (unsigned int i = 0; i < queue.size(); i++) {
        results.push_back(queue[i]);
    }
    int num = queue.size();
    queue.clear();
    return num;
}

int MessageQueue::flush(MessageReceiver *receiver) {
    boost::lock_guard<boost::mutex> guard(mtx);
    int num = queue.size();
    for (unsigned int i = 0; i < queue.size(); i++) {
        receiver->receive(queue[i]);
    }
    queue.clear();
    return num;
}
