#include "common.h"

#include "hexutil/messaging/message.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"
#include "hexgame/game/game_serialisation.h"


class GameMessageFactory: public AbstractMessageFactory {
public:
    GameMessageFactory(): AbstractMessageFactory(FirstGameMessage + 1, LastGameMessage - 1) {
        populate_names();
    }

    Message *new_message(int type) {
#define MSG_TYPE(s, c) if (type == s) { Message *m = new c; m->type = s; return m; } else
#include "hexgame/game/message_types.h"
#undef MSG_TYPE
        return nullptr;
    }

protected:
    void populate_names() {
#define MSG_TYPE(s, c) msg_type_names.push_back(#s);
#include "hexgame/game/message_types.h"
#undef MSG_TYPE
    }
};

static GameMessageFactory game_messages_factory;

void register_game_messages() {
    MessageTypeRegistry::add_factory(&game_messages_factory);
}
