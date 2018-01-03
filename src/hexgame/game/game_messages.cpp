#include "common.h"

#include "hexutil/messaging/message.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"
#include "hexgame/game/game_serialisation.h"


namespace hex {

#define MESSAGE_TEMPLATE_NAME Game
#define MESSAGE_TEMPLATE_INCLUDE_FILE "hexgame/game/message_types.h"
#define MESSAGE_TEMPLATE_START_ID 1000
#include "hexutil/messaging/message_cpp_template.h"


static GameMessageFactory game_messages_factory;

void register_game_messages() {
    MessageTypeRegistry::add_factory(&game_messages_factory);
}

};
