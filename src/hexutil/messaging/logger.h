#ifndef LOGGER_H
#define LOGGER_H

#include "hexutil/messaging/receiver.h"

class MessageLogger: public MessageReceiver {
public:
    MessageLogger(const std::string& prefix): prefix(prefix) { }
    virtual ~MessageLogger() { }

    virtual void receive(Message *update);

private:
    std::string prefix;
};

#endif
