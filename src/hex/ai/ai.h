#ifndef AI_H
#define AI_H

#include "hex/game/game.h"


class MessageReceiver;

class Ai {
public:
    Ai(Game *game, const std::string& faction_type, MessageReceiver *dispatcher);

    void update();
    void update_unit_stack(UnitStack& stack);

private:
    Game *game;
    std::string faction_type;
    MessageReceiver *dispatcher;
    unsigned int last_update;
    Faction::pointer faction;

    friend class AiUpdater;
};

#endif
