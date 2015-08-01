#include "common.h"

#include "hex/messaging/serialiser.h"
#include "hex/messaging/message.h"


Serialiser& operator<<(Serialiser& serialiser, const Message *msg) {
    if (msg->origin != 0 || msg->id != 0) {
        serialiser.begin_tuple();
        serialiser << msg->origin;
        serialiser << msg->id;
        serialiser.end_tuple();
    }
    std::string type_name = get_message_type_name(msg->type);
    serialiser.type_begin_tuple(type_name.c_str());
    msg->write(serialiser);
    serialiser.end_tuple();
    serialiser.end_record();

    return serialiser;
}

Deserialiser& operator>>(Deserialiser& deserialiser, Message *& msg) {
    int origin = 0;
    int id = 0;
    if (deserialiser.peek() == '(') {
        deserialiser.begin_tuple();
        deserialiser >> origin;
        deserialiser >> id;
        deserialiser.end_tuple();
    }
    std::string type_name;
    deserialiser.type_begin_tuple(type_name);
    int type = get_message_type(type_name);
    msg = new_message(type);
    if (msg == NULL) {
        deserialiser.error("Could not create class of type: %s", type_name.c_str());
        return deserialiser;
    }
    msg->origin = origin;
    msg->id = id;
    msg->read(deserialiser);
    deserialiser.end_tuple();
    deserialiser.end_record();

    return deserialiser;
}

