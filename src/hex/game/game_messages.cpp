#include "common.h"

#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/messaging/message.h"


#define MSG_TYPE(s, c) #s,
const char *msg_type_names[] = {
    "UndefinedMessageType",
#include "hex/game/message_types.h"
    "NUM_MESSAGE_TYPES"
};
#undef MSG_TYPE

int get_message_type(const std::string& name) {
    for (int i = 0; i < NUM_MESSAGE_TYPES; i++) {
        if (name == msg_type_names[i])
            return (MessageType) i;
    }
    return UndefinedMessageType;
}

const std::string get_message_type_name(int type) {
    if (type >= 0 && type < NUM_MESSAGE_TYPES) {
        return std::string(msg_type_names[type]);
    } else {
        return std::string("unknown");
    }
}

#define MSG_TYPE(s, c) if (type == s) { Message *m = new c; m->type = s; return m; } else
Message *new_message(int type) {
#include "hex/game/message_types.h"
    return NULL;
}
#undef MSG_TYPE
