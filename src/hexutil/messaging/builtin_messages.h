#ifndef BUILTIN_MESSAGES_H
#define BUILTIN_MESSAGES_H

#include "hexutil/basics/point.h"
#include "hexutil/basics/string_vector.h"
#include "hexutil/messaging/message.h"


enum BuiltinMessageType {
    FirstBuiltinMessage = 0,
#define MSG_TYPE(s, c) s,
#include "hexutil/messaging/message_types.h"
#undef MSG_TYPE
    LastBuiltinMessage
};


#define MSG_TYPE(s, c) typedef c s##Message;
#include "hexutil/messaging/message_types.h"
#undef MSG_TYPE


class BuiltinMessageFactory: public AbstractMessageFactory {
public:
    BuiltinMessageFactory();
    Message *new_message(int type);

protected:
    void populate_names();
};

void register_builtin_messages();

#endif
