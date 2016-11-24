#ifndef LEVEL_VIEW_UPDATER_H
#define LEVEL_VIEW_UPDATER_H

#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"


class Game;
class GameView;
class Resources;

class ViewUpdater: public MessageReceiver {
public:
    ViewUpdater(Game *game, GameView *game_view, Resources *resources);
    virtual ~ViewUpdater();

    virtual void receive(boost::shared_ptr<Message> update);

private:
    void apply_update(boost::shared_ptr<Message> update);

private:
    Game *game;
    GameView *game_view;
    Resources *resources;
};


#endif
