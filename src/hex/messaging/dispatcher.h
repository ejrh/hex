#ifndef DISPATCHER_H
#define DISPATCHER_H

class Dispatcher: public MessageReceiver {
public:
    Dispatcher(MessageReceiver *receiver): receiver(receiver) { }
    virtual void receive(boost::shared_ptr<Message> msg) { return receiver->receive(msg); }

private:
    MessageReceiver *receiver;
};


#endif
