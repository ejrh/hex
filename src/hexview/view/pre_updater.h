#ifndef PRE_UPDATER_H
#define PRE_UPDATER_H

#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"


class Game;
class GameView;
class BattleViewer;

class PreUpdater: public MessageReceiver {
public:
    PreUpdater(Game *game, GameView *game_view);
    virtual ~PreUpdater();

    virtual void receive(Message *update);

private:
    void apply_update(Message *update);

private:
    Game *game;
    GameView *game_view;

public:
    BattleViewer *battle_viewer;
};

#endif
