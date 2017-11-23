#include "common.h"

#include "hexutil/messaging/message.h"

#include "hexav/resources/resource_messages.h"


#include "hexav/resources/message_types.h"
#define MESSAGE_TEMPLATE_NAME Resource
#define MESSAGE_TEMPLATE_INCLUDE_FILE "hexav/resources/message_types.h"
#define MESSAGE_TEMPLATE_START_ID 2000
#include "hexutil/messaging/message_cpp_template.h"


static ResourceMessageFactory resource_messages_factory;

void register_resource_messages() {
    MessageTypeRegistry::add_factory(&resource_messages_factory);
}
