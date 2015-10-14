#include "common.h"

#include "hex/game/game.h"
#include "hex/game/game_arbiter.h"
#include "hex/game/game_messages.h"
#include "hex/game/movement/movement.h"
#include "hex/messaging/updater.h"


GameArbiter::GameArbiter(Game *game, MessageReceiver *publisher): game(game), publisher(publisher) {
}

GameArbiter::~GameArbiter() {
}

void GameArbiter::receive(boost::shared_ptr<Message> command) {

    switch (command->type) {
        case UnitMove: {
            boost::shared_ptr<UnitMoveMessage> cmd = boost::dynamic_pointer_cast<UnitMoveMessage>(command);
            int stack_id = cmd->data1;
            IntSet units = cmd->data2;
            Path& path = cmd->data3;
            int target_id = cmd->data4;

            UnitStack *stack = game->get_stack(stack_id);
            if (units.size() == 0 || path.size() < 2 || stack == NULL) {
                return;
            }

            /* Check that the move is allowed; shorten it if necessary */
            Point end_pos = path.back();
            UnitStack *end_stack = game->level.tiles[end_pos].stack;
            int end_stack_id = (end_stack != NULL) ? end_stack->id : 0;
            if (end_stack_id != target_id) {
                path.pop_back();
                target_id = 0;
            }

            MovementModel movement(&game->level);
            int allowed_steps = movement.check_path(stack, path);
            path.resize(allowed_steps + 1);

            if (path.size() < 2) {
                return;
            }

            /* Generate updates. */
            Faction *faction = stack->owner;
            bool move = units.size() == stack->units.size() && target_id == 0;
            bool split = units.size() < stack->units.size() && target_id == 0;
            bool merge = units.size() == stack->units.size() && target_id != 0;

            UnitStack *target = game->get_stack(target_id);
            bool attack = target != NULL && (target->owner != stack->owner);

            if (move)
                target_id = stack_id;
            if (split)
                target_id = game->get_free_stack_id();

            // If the stack is splitting to a new empty position, create a stack there
            if (split) {
                emit(create_message(CreateStack, target_id, end_pos, faction->id));
            }
            // Send the update
            emit(create_message(TransferUnits, stack_id, units, path, target_id));
            // If the whole stack merged with an existing one, destroy it
            if (merge) {
                emit(create_message(DestroyStack, stack_id));
            }

            if (attack) {
                BOOST_LOG_TRIVIAL(debug) << "Attack!";
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
