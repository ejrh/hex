#ifndef COUNTER_H
#define COUNTER_H

#include "hexutil/basics/statistics.h"
#include "hexutil/messaging/receiver.h"


class MessageCounter: public MessageReceiver {
public:
    MessageCounter(const std::string& prefix): prefix(prefix) { }

    virtual void receive(Message *update);

private:
    std::string prefix;
    std::unordered_map<int, Counter> counters;
};

#endif
