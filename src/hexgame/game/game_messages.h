#ifndef GAME_MESSAGES_H
#define GAME_MESSAGES_H

#include "hexutil/basics/point.h"
#include "hexutil/basics/string_vector.h"
#include "hexutil/messaging/message.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_serialisation.h"


#define MESSAGE_TEMPLATE_NAME Game
#define MESSAGE_TEMPLATE_INCLUDE_FILE "hexgame/game/message_types.h"
#define MESSAGE_TEMPLATE_START_ID 1000
#include "hexutil/messaging/message_h_template.h"


void register_game_messages();

#endif
