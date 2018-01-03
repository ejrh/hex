#include "common.h"

#include "hexutil/messaging/message.h"
#include "hexgame/game/throttle.h"


namespace hex {

Throttle::Throttle(MessageReceiver *receiver):
         receiver(receiver) {
}


Throttle::~Throttle() {
    check_blocked_messages();
    if (!blocked_messages.empty()) {
        BOOST_LOG_TRIVIAL(error) << boost::format("Throttle is being destroyed with %d blocked messages remaining!") % blocked_messages.size();
    }
}


void Throttle::lock_stack(int id) {
    //std::cerr << "lock stack " << id << "\n";
    locked_stacks.insert(id);
}


void Throttle::unlock_stack(int id) {
    //std::cerr << "unlock stack " << id << "\n";
    locked_stacks.erase(id);
    check_blocked_messages();
}


void Throttle::receive(Message *msg) {
    if (is_message_blocked(msg) || !blocked_messages.empty()) {
        //std::cerr << "message is blocked: " << msg << "\n";
        blocked_messages.push(msg->shared_from_this());
        check_blocked_messages();
    } else {
        receiver->receive(msg);
    }
}


bool Throttle::is_message_blocked(Message *msg) const {
    if (!locked_stacks.empty())
        return true;

    return false;
}


void Throttle::check_blocked_messages() {
    while (!blocked_messages.empty()) {
        const boost::shared_ptr<Message> message = blocked_messages.front();
        if (is_message_blocked(message.get()))
            break;
        blocked_messages.pop();
        //std::cerr << "message is unblocked: " << message << "\n";
        receiver->receive(message);
    }
}

};
