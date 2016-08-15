#ifndef GAME_MESSAGES_H
#define GAME_MESSAGES_H

#include "hex/basics/point.h"
#include "hex/basics/string_vector.h"
#include "hex/game/game.h"
#include "hex/game/game_serialisation.h"
#include "hex/messaging/message.h"
#include "hex/resources/view_def.h"
#include "hex/resources/view_def_serialisation.h"


#define MSG_TYPE(s, c) s,
enum MessageType {
    UndefinedMessageType,
#include "hex/game/message_types.h"
    NUM_MESSAGE_TYPES
};
#undef MSG_TYPE


#define MSG_TYPE(s, c) typedef c s##Message;
#include "hex/game/message_types.h"
#undef MSG_TYPE

#define create_message(s, ...) boost::make_shared<s ## Message>(s, ##__VA_ARGS__)

#endif
