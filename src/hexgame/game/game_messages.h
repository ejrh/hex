#ifndef GAME_MESSAGES_H
#define GAME_MESSAGES_H

#include "hexutil/basics/point.h"
#include "hexutil/basics/string_vector.h"
#include "hexutil/messaging/message.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_serialisation.h"


enum GameMessageType {
    FirstGameMessage = 1000,
#define MSG_TYPE(s, c) s,
#include "hexgame/game/message_types.h"
#undef MSG_TYPE
    LastGameMessage
};


#define MSG_TYPE(s, c) typedef c s##Message;
#include "hexgame/game/message_types.h"
#undef MSG_TYPE

void register_game_messages();

#endif
