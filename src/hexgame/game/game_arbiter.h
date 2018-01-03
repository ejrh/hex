#ifndef GAME_ARBITER_H
#define GAME_ARBITER_H

#include "hexutil/basics/statistics.h"
#include "hexutil/messaging/counter.h"
#include "hexutil/messaging/receiver.h"


namespace hex {

class Game;
class Updater;

class GameArbiter: public MessageReceiver {
public:
    GameArbiter(Game *game, MessageReceiver *publisher);
    virtual ~GameArbiter();

    virtual void receive(Message *update);
    using MessageReceiver::receive;

private:
    void process_command(Message *update);
    void spawn_units();
    void emit(boost::shared_ptr<Message> update);

private:
    Game *game;
    MessageReceiver *publisher;

    MessageCounter command_counter;
    MessageCounter update_counter;
};

};

#endif
