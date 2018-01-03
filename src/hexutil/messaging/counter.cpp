#include "common.h"

#include "hexutil/messaging/counter.h"
#include "hexutil/messaging/message.h"


namespace hex {

void MessageCounter::receive(Message *msg) {
    auto found = counters.find(msg->type);
    if (found == counters.end()) {
        std::string name = prefix + "." + MessageTypeRegistry::get_message_type_name(msg->type);
        counters[msg->type] = Counter(name, 1);
    } else {
        ++found->second;
    }
}

};
