#ifndef GAME_ARBITER_H
#define GAME_ARBITER_H

#include "hexutil/basics/statistics.h"
#include "hexutil/messaging/counter.h"
#include "hexutil/messaging/logger.h"
#include "hexutil/messaging/publisher.h"
#include "hexutil/messaging/receiver.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_updater.h"


namespace hex {

class GameArbiter: public MessageReceiver {
public:
    GameArbiter(Game *game, MessageReceiver *publisher);
    virtual ~GameArbiter();

    MessageReceiver& get_emitter();

    virtual void receive(Message *update);
    using MessageReceiver::receive;

private:
    void process_command(Message *update);
    void spawn_units();
    void emit(const boost::shared_ptr<Message>& update);

private:
    Game *game;
    GameUpdater game_updater;

    Publisher emitter;

    MessageLogger command_logger;
    MessageCounter command_counter;
    MessageLogger update_logger;
    MessageCounter update_counter;
};

};

#endif
