#include "common.h"

#include "hex/basics/error.h"
#include "hex/game/game.h"
#include "hex/game/game_arbiter.h"
#include "hex/game/game_messages.h"
#include "hex/game/combat/combat.h"
#include "hex/game/movement/movement.h"
#include "hex/messaging/updater.h"


GameArbiter::GameArbiter(Game *game, MessageReceiver *publisher): game(game), publisher(publisher) {
}

GameArbiter::~GameArbiter() {
}

void GameArbiter::receive(boost::shared_ptr<Message> command) {
    try {
        process_command(command);
    } catch (const DataError& err) {
        BOOST_LOG_TRIVIAL(error) << "Invalid command received; " << err.what();
    }
}

void GameArbiter::process_command(boost::shared_ptr<Message> command) {
    switch (command->type) {
        case UnitMove: {
            boost::shared_ptr<UnitMoveMessage> cmd = boost::dynamic_pointer_cast<UnitMoveMessage>(command);
            int stack_id = cmd->data1;
            IntSet units = cmd->data2;
            Path& path = cmd->data3;
            int target_id = cmd->data4;

            UnitStack::pointer stack = game->stacks.get(stack_id);
            if (units.empty() || !stack->has_units(units) || path.empty()) {
                throw DataError() << "Invalid UnitMove message";
            }

            /* Check that the move is allowed; shorten it if necessary */
            Point end_pos = path.back();
            UnitStack::pointer end_stack = game->level.tiles[end_pos].stack;
            int end_stack_id = end_stack ? end_stack->id : 0;
            if (end_stack_id != target_id) {
                path.pop_back();
                target_id = 0;
            }

            MovementModel movement(&game->level);
            unsigned int allowed_steps = movement.check_path(*stack, path);
            bool truncated = allowed_steps < path.size();
            int attack_target_id = target_id;
            if (truncated)
                target_id = 0;
            path.resize(allowed_steps);

            if (!path.empty()) {
                /* Generate updates. */
                Faction::pointer faction = stack->owner;
                bool move = units.size() == stack->units.size() && target_id == 0;
                bool split = units.size() < stack->units.size() && target_id == 0;
                bool merge = units.size() == stack->units.size() && target_id != 0;

                UnitStack::pointer target = game->stacks.find(target_id);

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
            }

            UnitStack::pointer attack_target = game->stacks.find(attack_target_id);
            bool attack = attack_target && (attack_target->owner != stack->owner);
            if (attack) {
                BOOST_LOG_TRIVIAL(debug) << "Attack!";
                Point target_point = attack_target->position;
                Point attacking_point = end_pos;
                Battle battle(game, target_point, attacking_point);
                battle.run();
                emit(create_message(DoBattle, end_stack_id, target_point, battle.moves));
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
