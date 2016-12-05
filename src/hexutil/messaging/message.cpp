#include "common.h"

#include "hexutil/messaging/message.h"
#include "hexutil/messaging/serialiser.h"


Serialiser& operator<<(Serialiser& serialiser, const Message *msg) {
    if (msg->origin != 0 || msg->id != 0) {
        serialiser.begin_tuple();
        serialiser << msg->origin;
        serialiser << msg->id;
        serialiser.end_tuple();
    }
    std::string type_name = MessageTypeRegistry::get_message_type_name(msg->type);
    serialiser.type_begin_tuple(type_name.c_str());
    msg->write(serialiser);
    serialiser.end_tuple();

    return serialiser;
}

Deserialiser& operator>>(Deserialiser& deserialiser, Message *& msg) {
    int origin = 0;
    int id = 0;
    deserialiser.skip_to_next();
    if (deserialiser.peek() == '(') {
        deserialiser.begin_tuple();
        deserialiser >> origin;
        deserialiser >> id;
        deserialiser.end_tuple();
    }
    std::string type_name;
    deserialiser.type_begin_tuple(type_name);
    int type = MessageTypeRegistry::get_message_type(type_name);
    msg = MessageTypeRegistry::new_message(type);
    if (msg == NULL) {
        deserialiser.error("Could not create class of type: %s", type_name.c_str());
        return deserialiser;
    }
    msg->origin = origin;
    msg->id = id;
    msg->read(deserialiser);
    deserialiser.end_tuple();

    return deserialiser;
}

Serialiser& operator<<(Serialiser& serialiser, const MessageSequence& sequence) {
    std::vector<Message *> msgs;
    for (auto iter = sequence.begin(); iter != sequence.end(); iter++)
        msgs.push_back(iter->get());
    serialiser << msgs;
    return serialiser;
}

Deserialiser& operator>>(Deserialiser& deserialiser, MessageSequence& sequence) {
    std::vector<Message *> msgs;
    deserialiser >> msgs;
    sequence.clear();
    for (auto iter = msgs.begin(); iter != msgs.end(); iter++)
        sequence.push_back(boost::shared_ptr<Message>(*iter));
    return deserialiser;
}


const std::string MessageFactory::empty_string;


MessageTypeRegistry MessageTypeRegistry::instance;
std::string MessageTypeRegistry::undefined_message_type_str("UndefinedMessageType");


void MessageTypeRegistry::add_factory(MessageFactory *factory) {
    instance.factories.push_back(factory);
}

int MessageTypeRegistry::get_message_type(const std::string& name) {
    for (auto iter = instance.factories.begin(); iter != instance.factories.end(); iter++) {
        MessageFactory *factory = *iter;
        int rv = factory->get_message_type(name);
        if (rv != UndefinedMessageType)
            return rv;
    }

    return UndefinedMessageType;
}

const std::string& MessageTypeRegistry::get_message_type_name(int type) {
    for (auto iter = instance.factories.begin(); iter != instance.factories.end(); iter++) {
        MessageFactory *factory = *iter;
        if (type >= factory->min_type() && type <= factory->max_type()) {
            const std::string& rv = factory->get_message_type_name(type);
            if (rv != MessageFactory::empty_string)
                return rv;
        }
    }

    return undefined_message_type_str;
}

Message *MessageTypeRegistry::new_message(int type) {
    for (auto iter = instance.factories.begin(); iter != instance.factories.end(); iter++) {
        MessageFactory *factory = *iter;
        if (type >= factory->min_type() && type <= factory->max_type()) {
            Message *rv = factory->new_message(type);
            if (rv != nullptr)
                return rv;
        }
    }

    return nullptr;
}
