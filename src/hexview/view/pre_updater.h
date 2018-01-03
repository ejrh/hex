#ifndef PRE_UPDATER_H
#define PRE_UPDATER_H

#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"


namespace hex {

class Game;
class GameView;
class CombatScreen;

class PreUpdater: public MessageReceiver {
public:
    PreUpdater(Game *game);
    virtual ~PreUpdater();

    virtual void receive(Message *update);

private:
    void apply_update(Message *update);

private:
    Game *game;

public:
    CombatScreen *combat_screen;
};

};

#endif
