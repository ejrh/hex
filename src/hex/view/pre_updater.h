#ifndef PRE_UPDATER_H
#define PRE_UPDATER_H

#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"


class Game;
class GameView;

class PreUpdater: public MessageReceiver {
public:
    PreUpdater(Game *game, GameView *game_view);
    virtual ~PreUpdater();

    virtual void receive(boost::shared_ptr<Message> update);

private:
    void apply_update(boost::shared_ptr<Message> update);

private:
    Game *game;
    GameView *game_view;
};

#endif
