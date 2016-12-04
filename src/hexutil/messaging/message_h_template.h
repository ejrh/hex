#ifndef MESSAGE_TEMPLATE_NAME
#error "MESSAGE_TEMPLATE_NAME macro not defined before inclusion of message_h_template.h"
#endif

#ifndef MESSAGE_TEMPLATE_INCLUDE_FILE
#error "MESSAGE_TEMPLATE_INCLUDE_FILE macro not defined before inclusion of message_h_template.h"
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

enum CONCAT2(MESSAGE_TEMPLATE_NAME, MessageType) {
    MESSAGE_TEMPLATE_FIRST = MESSAGE_TEMPLATE_START_ID,
#define MSG_TYPE(s, c) s,
#include MESSAGE_TEMPLATE_INCLUDE_FILE
#undef MSG_TYPE
    MESSAGE_TEMPLATE_LAST
};


#define MSG_TYPE(s, c) typedef c s##Message;
#include MESSAGE_TEMPLATE_INCLUDE_FILE
#undef MSG_TYPE


class MESSAGE_TEMPLATE_CLASS_NAME: public AbstractMessageFactory {
public:
    MESSAGE_TEMPLATE_CLASS_NAME();
    Message *new_message(int type);

protected:
    void populate_names();
};

#undef MESSAGE_TEMPLATE_LAST
#undef MESSAGE_TEMPLATE_FIRST
#undef MESSAGE_TEMPLATE_CLASS_NAME
#undef MESSAGE_TEMPLATE_START_ID
#undef MESSAGE_TEMPLATE_INCLUDE_FILE
#undef MESSAGE_TEMPLATE_NAME
