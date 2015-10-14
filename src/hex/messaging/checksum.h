#ifndef CHECKSUM_H
#define CHECKSUM_H

#include "hex/messaging/receiver.h"


class MessageChecksum: public MessageReceiver {
public:
    MessageChecksum(): checksum(0) { }

    virtual void receive(boost::shared_ptr<Message> msg);

public:
    unsigned long checksum;
};

#endif
