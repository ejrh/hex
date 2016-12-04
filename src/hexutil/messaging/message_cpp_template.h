#ifndef MESSAGE_TEMPLATE_NAME
#error "MESSAGE_TEMPLATE_NAME macro not defined before inclusion of message_cpp_template.h"
#endif

#ifndef MESSAGE_TEMPLATE_INCLUDE_FILE
#error "MESSAGE_TEMPLATE_INCLUDE_FILE macro not defined before inclusion of message_cpp_template.h"
#endif

#ifndef MESSAGE_TEMPLATE_START_ID
#error "MESSAGE_TEMPLATE_START_ID macro not defined before inclusion of message_h_template.h"
#endif


#define PASTE2(x,y) x##y
#define CONCAT2(x,y) PASTE2(x,y)

#define PASTE3(x,y,z) x##y##z
#define CONCAT3(x,y,z) PASTE3(x,y,z)


#define MESSAGE_TEMPLATE_CLASS_NAME CONCAT2(MESSAGE_TEMPLATE_NAME, MessageFactory)
#define MESSAGE_TEMPLATE_FIRST CONCAT3(First, MESSAGE_TEMPLATE_NAME, Message)
#define MESSAGE_TEMPLATE_LAST CONCAT3(Last, MESSAGE_TEMPLATE_NAME, Message)


MESSAGE_TEMPLATE_CLASS_NAME::MESSAGE_TEMPLATE_CLASS_NAME(): AbstractMessageFactory(MESSAGE_TEMPLATE_FIRST + 1, MESSAGE_TEMPLATE_LAST - 1) {
    populate_names();
}

Message *MESSAGE_TEMPLATE_CLASS_NAME::new_message(int type) {
#define MSG_TYPE(s, c) if (type == s) { Message *m = new c; m->type = s; return m; } else
#include MESSAGE_TEMPLATE_INCLUDE_FILE
#undef MSG_TYPE
    return nullptr;
}

void MESSAGE_TEMPLATE_CLASS_NAME::populate_names() {
#define MSG_TYPE(s, c) msg_type_names.push_back(#s);
#include MESSAGE_TEMPLATE_INCLUDE_FILE
#undef MSG_TYPE
}

#undef MESSAGE_TEMPLATE_LAST
#undef MESSAGE_TEMPLATE_FIRST
#undef MESSAGE_TEMPLATE_CLASS_NAME
#undef MESSAGE_TEMPLATE_START_ID
#undef MESSAGE_TEMPLATE_INCLUDE_FILE
#undef MESSAGE_TEMPLATE_NAME
