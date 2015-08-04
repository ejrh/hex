#ifndef WRITER_H
#define WRITER_H

#include "hex/messaging/receiver.h"


class MessageWriter: public MessageReceiver {
public:
    MessageWriter(std::ostream& os): writer(os) { }

    virtual void receive(boost::shared_ptr<Message> msg) { writer << msg.get(); }

private:
    Serialiser writer;
};

#endif
