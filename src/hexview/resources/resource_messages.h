#ifndef RESOURCE_MESSAGES_H
#define RESOURCE_MESSAGES_H

#include "hexutil/messaging/message.h"

#include "hexview/resources/view_def.h"
#include "hexview/resources/view_def_serialisation.h"


#define MESSAGE_TEMPLATE_NAME Resource
#define MESSAGE_TEMPLATE_INCLUDE_FILE "hexview/resources/message_types.h"
#define MESSAGE_TEMPLATE_START_ID 2000
#include "hexutil/messaging/message_h_template.h"


void register_resource_messages();

#endif
