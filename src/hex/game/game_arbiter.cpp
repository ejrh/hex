#include "common.h"

#include "hex/messaging/serialiser.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"
#include "hex/messaging/updater.h"

#include "hex/game/game.h"
#include "hex/game/game_serialisation.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_arbiter.h"


GameArbiter::GameArbiter(Game *game, Updater *publisher): game(game), publisher(publisher) {
}

GameArbiter::~GameArbiter() {
}

void GameArbiter::receive(boost::shared_ptr<Message> command) {

    switch (command->type) {
        case UnitMove: {
            boost::shared_ptr<UnitMoveMessage> cmd = boost::dynamic_pointer_cast<UnitMoveMessage>(command);

            // TODO check that move is allowed, refuse if not
            Point end_pos = cmd->data2.back();
            UnitStack *end_stack = game->level.tiles[end_pos].stack;
            int end_stack_id = (end_stack != NULL) ? end_stack->id : 0;
            if (end_stack_id != cmd->data3) {
                cmd->data2.pop_back();
                cmd->data3 = 0;
            }

            if (cmd->data2.size() < 2)
                return;

            emit(create_message(UnitMove, cmd->data1, cmd->data2, cmd->data3));
        } break;

        case Chat: {
            boost::shared_ptr<WrapperMessage<std::string> > chat_msg = boost::dynamic_pointer_cast<WrapperMessage<std::string> >(command);
            emit(create_message(Chat, chat_msg->data));
        } break;

        default:
            break;
    }
}

void GameArbiter::emit(boost::shared_ptr<Message> update) {
    publisher->receive(update);
}
