#include "common.h"

#include "hexutil/messaging/message.h"

#include "hexview/view/view_resource_messages.h"


namespace hex {

#include "hexview/view/message_types.h"
#define MESSAGE_TEMPLATE_NAME ViewResource
#define MESSAGE_TEMPLATE_INCLUDE_FILE "hexview/view/message_types.h"
#define MESSAGE_TEMPLATE_START_ID 3000
#include "hexutil/messaging/message_cpp_template.h"


static ViewResourceMessageFactory view_resource_messages_factory;

void register_view_resource_messages() {
    MessageTypeRegistry::add_factory(&view_resource_messages_factory);
}

};
