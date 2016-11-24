#include "common.h"

#include "hexutil/messaging/message.h"
#include "hexutil/messaging/builtin_messages.h"


BuiltinMessageFactory::BuiltinMessageFactory(): AbstractMessageFactory(FirstBuiltinMessage + 1, LastBuiltinMessage - 1) {
    populate_names();
}

Message *BuiltinMessageFactory::new_message(int type) {
#define MSG_TYPE(s, c) if (type == s) { Message *m = new c; m->type = s; return m; } else
#include "hexutil/messaging/message_types.h"
#undef MSG_TYPE
    return nullptr;
}

void BuiltinMessageFactory::populate_names() {
#define MSG_TYPE(s, c) msg_type_names.push_back(#s);
#include "hexutil/messaging/message_types.h"
#undef MSG_TYPE
}

static BuiltinMessageFactory builtin_messages_factory;

void register_builtin_messages() {
    MessageTypeRegistry::add_factory(&builtin_messages_factory);
}
