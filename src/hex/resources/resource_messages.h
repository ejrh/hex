#ifndef RESOURCE_MESSAGES_H
#define RESOURCE_MESSAGES_H

#include "hexutil/messaging/message.h"

#include "hex/resources/view_def.h"
#include "hex/resources/view_def_serialisation.h"


enum ResourceMessageType {
    FirstResourceMessage = 2000,
#define MSG_TYPE(s, c) s,
#include "hex/resources/message_types.h"
#undef MSG_TYPE
    LastResourceMessage
};


#define MSG_TYPE(s, c) typedef c s##Message;
#include "hex/resources/message_types.h"
#undef MSG_TYPE

void register_resource_messages();

#endif
