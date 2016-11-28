#ifndef CHECKSUM_H
#define CHECKSUM_H

#include "hexutil/messaging/receiver.h"


class MessageChecksum: public MessageReceiver {
public:
    MessageChecksum(): checksum(0) { }

    virtual void receive(Message *msg);

public:
    unsigned long checksum;
};

#endif
