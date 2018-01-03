#include "common.h"

#include "hexutil/messaging/message.h"
#include "hexutil/messaging/builtin_messages.h"


namespace hex {

#define MESSAGE_TEMPLATE_NAME Builtin
#define MESSAGE_TEMPLATE_INCLUDE_FILE "hexutil/messaging/message_types.h"
#define MESSAGE_TEMPLATE_START_ID 0
#include "hexutil/messaging/message_cpp_template.h"


static BuiltinMessageFactory builtin_messages_factory;

void register_builtin_messages() {
    MessageTypeRegistry::add_factory(&builtin_messages_factory);
}

};
