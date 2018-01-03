#ifndef GAME_UPDATER_H
#define GAME_UPDATER_H

#include "hexutil/messaging/receiver.h"


namespace hex {

class Game;

class GameUpdater: public MessageReceiver {
public:
    GameUpdater(Game *game);
    virtual ~GameUpdater();

    virtual void receive(Message *update);

private:
    void apply_update(Message *update);

private:
    Game *game;
};

};

#endif
