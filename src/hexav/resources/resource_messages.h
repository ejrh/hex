#ifndef RESOURCE_MESSAGES_H
#define RESOURCE_MESSAGES_H

#include "hexutil/messaging/message.h"

#include "hexav/resources/image_ref.h"


#include "hexav/resources/message_types.h"
#define MESSAGE_TEMPLATE_NAME Resource
#define MESSAGE_TEMPLATE_INCLUDE_FILE "hexav/resources/message_types.h"
#define MESSAGE_TEMPLATE_START_ID 2000
#include "hexutil/messaging/message_h_template.h"


void register_resource_messages();

#endif
