#ifndef LEVEL_VIEW_UPDATER_H
#define LEVEL_VIEW_UPDATER_H

#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"


namespace hex {

class Game;
class GameView;
class Resources;

class ViewUpdater: public MessageReceiver {
public:
    ViewUpdater(Game *game, GameView *game_view, ViewResources *resources);
    virtual ~ViewUpdater();

    virtual void receive(Message *update);

private:
    void apply_update(Message *update);

private:
    Game *game;
    GameView *game_view;
    ViewResources *resources;
};

};

#endif
