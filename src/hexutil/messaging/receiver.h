#ifndef RECEIVER_H
#define RECEIVER_H

namespace hex {

class Message;

class MessageReceiver {
public:
    virtual ~MessageReceiver() { }
    virtual void receive(Message *msg) = 0;
    void receive(const boost::shared_ptr<Message>& msg) {
        receive(msg.get());
    };
};

extern void replay_messages(const std::string& filename, MessageReceiver& receiver);
extern void replay_messages(std::istream& input, MessageReceiver& receiver, const std::string& input_name);

};

#endif
