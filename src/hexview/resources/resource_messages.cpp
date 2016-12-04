#include "common.h"

#include "hexutil/messaging/message.h"

#include "hexview/resources/resource_messages.h"


class ResourceMessageFactory: public AbstractMessageFactory {
public:
    ResourceMessageFactory(): AbstractMessageFactory(FirstResourceMessage + 1, LastResourceMessage - 1) {
        populate_names();
    }

    Message *new_message(int type) {
#define MSG_TYPE(s, c) if (type == s) { Message *m = new c; m->type = s; return m; } else
#include "hexview/resources/message_types.h"
#undef MSG_TYPE
        return nullptr;
    }

protected:
    void populate_names() {
#define MSG_TYPE(s, c) msg_type_names.push_back(#s);
#include "hexview/resources/message_types.h"
#undef MSG_TYPE
    }
};

static ResourceMessageFactory resource_messages_factory;

void register_resource_messages() {
    MessageTypeRegistry::add_factory(&resource_messages_factory);
}
