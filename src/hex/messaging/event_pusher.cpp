#include "common.h"

#include "hex/messaging/event_pusher.h"

EventPusher::EventPusher() {
    event_type = SDL_RegisterEvents(1);
}

EventPusher::~EventPusher() { }

void EventPusher::receive(boost::shared_ptr<Message> msg) {
    SDL_Event user_event;

    user_event.type = event_type;
    new (&user_event.user.data1) boost::shared_ptr<Message>(msg);
    SDL_PushEvent(&user_event);
}

boost::shared_ptr<Message> EventPusher::get_message(SDL_Event& evt) {
    boost::shared_ptr<Message> msg = *reinterpret_cast<boost::shared_ptr<Message> *>(&evt.user.data1);
    (reinterpret_cast<boost::shared_ptr<Message> *>(&evt.user.data1))->~shared_ptr();
    return msg;
}
