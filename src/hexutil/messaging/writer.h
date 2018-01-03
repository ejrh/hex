#ifndef WRITER_H
#define WRITER_H

#include "hexutil/messaging/receiver.h"


namespace hex {

class MessageWriter: public MessageReceiver {
public:
    MessageWriter(std::ostream& os): writer(os) { }

    virtual void receive(Message *msg) {
        writer << msg;
        writer.end_record();
    }

private:
    Serialiser writer;
};

};

#endif
