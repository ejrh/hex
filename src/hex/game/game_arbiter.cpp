#include "common.h"

#include "hex/game/game.h"
#include "hex/game/game_arbiter.h"
#include "hex/game/game_messages.h"
#include "hex/messaging/updater.h"


GameArbiter::GameArbiter(Game *game, MessageReceiver *publisher): game(game), publisher(publisher) {
}

GameArbiter::~GameArbiter() {
}

void GameArbiter::receive(boost::shared_ptr<Message> command) {

    switch (command->type) {
        case UnitMove: {
            boost::shared_ptr<UnitMoveMessage> cmd = boost::dynamic_pointer_cast<UnitMoveMessage>(command);

            if (cmd->data2.size() == 0)
                return;

            // TODO check that move is allowed, refuse if not
            Point end_pos = cmd->data3.back();
            UnitStack *end_stack = game->level.tiles[end_pos].stack;
            int end_stack_id = (end_stack != NULL) ? end_stack->id : 0;
            if (end_stack_id != cmd->data4) {
                cmd->data3.pop_back();
                cmd->data4 = 0;
            }

            if (cmd->data3.size() < 2)
                return;

            UnitStack *stack = game->get_stack(cmd->data1);
            Faction *faction = stack->owner;
            bool move = cmd->data2.size() == stack->units.size() && cmd->data4 == 0;
            bool split = cmd->data2.size() < stack->units.size() && cmd->data4 == 0;
            bool merge = cmd->data2.size() == stack->units.size() && cmd->data4 != 0;

            int target_id = cmd->data4;
            if (move)
                target_id = cmd->data1;
            if (split)
                target_id = game->get_free_stack_id();

            // If the stack is splitting to a new empty position, create a stack there
            if (split) {
                emit(create_message(CreateStack, target_id, end_pos, faction->id));
            }
            // Send the update
            emit(create_message(TransferUnits, cmd->data1, cmd->data2, cmd->data3, target_id));
            // If the whole stack merged with an existing one, destroy it
            if (merge) {
                emit(create_message(DestroyStack, cmd->data1));
            }
        } break;

        case FactionReady: {
            boost::shared_ptr<FactionReadyMessage> cmd = boost::dynamic_pointer_cast<FactionReadyMessage>(command);
            int faction_id = cmd->data1;
            bool ready = cmd->data2;
            if (game->mark_faction_ready(faction_id, ready)) {
                emit(create_message(FactionReady, faction_id, ready));
            }

            if (game->all_factions_ready()) {
                emit(create_message(TurnEnd));
                // process turn end
                game->turn_number++;
                emit(create_message(TurnBegin, game->turn_number));
            }
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
