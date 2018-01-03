#ifndef THROTTLE_H
#define THROTTLE_H

#include "hexutil/messaging/receiver.h"


namespace hex {

class Throttle: public MessageReceiver {
public:
    Throttle(MessageReceiver *receiver);
    ~Throttle();

    void lock_stack(int id);
    void unlock_stack(int id);

    virtual void receive(Message *msg);
    using MessageReceiver::receive;

private:
    bool is_message_blocked(Message *msg) const;
    void check_blocked_messages();

private:
    MessageReceiver *receiver;
    std::queue<boost::shared_ptr<Message> > blocked_messages;
    std::unordered_set<int> locked_stacks;
};

};

#endif
