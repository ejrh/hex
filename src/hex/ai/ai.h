#ifndef AI_H
#define AI_H

class Game;
class MessageReceiver;
class UnitStack;
class Faction;

class Ai {
public:
    Ai(Game *game, const std::string& faction_type, MessageReceiver *dispatcher);

    void update();
    void update_unit_stack(UnitStack *stack);

private:
    Game *game;
    std::string faction_type;
    MessageReceiver *dispatcher;
    unsigned int last_update;
    Faction *faction;

    friend class AiUpdater;
};

#endif
