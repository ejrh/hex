#ifndef BUILTIN_MESSAGES_H
#define BUILTIN_MESSAGES_H

#include "hexutil/basics/datum.h"
#include "hexutil/basics/point.h"
#include "hexutil/basics/string_vector.h"
#include "hexutil/messaging/message.h"


namespace hex {

#define MESSAGE_TEMPLATE_NAME Builtin
#define MESSAGE_TEMPLATE_INCLUDE_FILE "hexutil/messaging/message_types.h"
#define MESSAGE_TEMPLATE_START_ID 0
#include "hexutil/messaging/message_h_template.h"


void register_builtin_messages();

};

#endif
