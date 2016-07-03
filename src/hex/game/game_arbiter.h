#ifndef GAME_ARBITER_H
#define GAME_ARBITER_H

#include "hex/messaging/receiver.h"


class Game;
class Updater;

class GameArbiter: public MessageReceiver {
public:
    GameArbiter(Game *game, MessageReceiver *publisher);
    virtual ~GameArbiter();

    virtual void receive(boost::shared_ptr<Message> update);

private:
    void process_command(boost::shared_ptr<Message> update);
    void spawn_units();
    void emit(boost::shared_ptr<Message> update);

private:
    Game *game;
    MessageReceiver *publisher;
};


#endif
