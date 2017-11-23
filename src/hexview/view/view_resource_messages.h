#ifndef VIEW_RESOURCE_MESSAGES_H
#define VIEW_RESOURCE_MESSAGES_H

#include "hexutil/messaging/message.h"

#include "hexview/view/view_def.h"
#include "hexview/view/view_def_serialisation.h"


#include "hexview/view/message_types.h"
#define MESSAGE_TEMPLATE_NAME ViewResource
#define MESSAGE_TEMPLATE_INCLUDE_FILE "hexview/view/message_types.h"
#define MESSAGE_TEMPLATE_START_ID 3000
#include "hexutil/messaging/message_h_template.h"


void register_view_resource_messages();

#endif
