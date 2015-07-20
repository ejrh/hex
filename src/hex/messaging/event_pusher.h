#ifndef EVENT_PUSHER_H
#define EVENT_PUSHER_H

#include "hex/messaging/receiver.h"

class EventPusher: public MessageReceiver {
public:
    EventPusher();
    virtual ~EventPusher();

    virtual void receive(boost::shared_ptr<Message> msg);
    boost::shared_ptr<Message> get_message(SDL_Event& evt);

public:
    Uint32 event_type;
};

#endif
