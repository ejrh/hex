#ifndef COUNTER_H
#define COUNTER_H

#include "hexutil/basics/atom.h"
#include "hexutil/basics/statistics.h"
#include "hexutil/messaging/receiver.h"


namespace hex {

class MessageCounter: public MessageReceiver {
public:
    MessageCounter(const Atom prefix): prefix(prefix) { }

    virtual void receive(Message *update);

private:
    Atom prefix;
    std::unordered_map<int, Counter> counters;
};

};

#endif
