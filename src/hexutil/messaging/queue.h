#ifndef QUEUE_H
#define QUEUE_H

#include "hexutil/messaging/receiver.h"


namespace hex {

class MessageQueue: public MessageReceiver {
public:
    MessageQueue(unsigned int capacity);
    virtual ~MessageQueue();
    virtual void receive(Message *msg);

    boost::shared_ptr<Message> fetch_message();
    int fetch_messages(std::vector<boost::shared_ptr<Message> >& results);
    int flush(MessageReceiver *receiver);
    const std::vector<boost::shared_ptr<Message> >& get_queue() const { return queue; }

private:
    boost::mutex mtx;
    unsigned int capacity;
    std::vector<boost::shared_ptr<Message> > queue;
};

};

#endif
