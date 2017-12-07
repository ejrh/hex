#include "common.h"

#include "hexutil/basics/error.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_arbiter.h"
#include "hexgame/game/game_messages.h"
#include "hexgame/game/combat/combat.h"
#include "hexgame/game/movement/movement.h"


GameArbiter::GameArbiter(Game *game, MessageReceiver *publisher):
        game(game), publisher(publisher),
        command_counter("arbiter.command"), update_counter("arbiter.update") {
}

GameArbiter::~GameArbiter() {
}

void GameArbiter::receive(Message *command) {
    try {
        process_command(command);
        command_counter.receive(command);
    } catch (const DataError& err) {
        BOOST_LOG_TRIVIAL(error) << "Invalid command received; " << err.what();
    }
}

void GameArbiter::process_command(Message *command) {
    switch (command->type) {
        case UnitMove: {
            auto cmd = dynamic_cast<UnitMoveMessage *>(command);
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

            MovementModel movement(game);
            UnitStack::pointer selected_stack = stack->copy_subset(units);
            unsigned int allowed_steps = movement.check_path(*selected_stack, path);
            bool truncated = allowed_steps < path.size();
            int attack_target_id = target_id;
            if (truncated)
                target_id = 0;
            path.resize(allowed_steps);

            if (!path.empty()) {
                end_pos = path.back();
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

                // Send the moves
                for (auto iter = path.begin(); iter != path.end(); iter++) {
                    emit(create_message(MoveUnits, stack_id, units, *iter));
                }
                // If the stack is splitting to a new empty position, create a stack there
                if (split) {
                    emit(create_message(CreateStack, target_id, end_pos, faction->id));
                }
                emit(create_message(TransferUnits, stack_id, units, path, target_id));
                // If the whole stack merged with an existing one, destroy it
                if (merge) {
                    emit(create_message(DestroyStack, stack_id));
                }
            } else {
                end_pos = stack->position;
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
            auto cmd = dynamic_cast<FactionReadyMessage *>(command);
            int faction_id = cmd->data1;
            bool ready = cmd->data2;
            if (game->mark_faction_ready(faction_id, ready)) {
                emit(create_message(FactionReady, faction_id, ready));
            }

            if (game->all_factions_ready()) {
                emit(create_message(TurnEnd));
                // process turn end

                spawn_units();
                game->turn_number++;
                emit(create_message(TurnBegin, game->turn_number));
            }
        } break;

        case Chat: {
            auto chat_msg = dynamic_cast<ChatMessage *>(command);
            emit(create_message(Chat, chat_msg->data));
        } break;

        case SetLevelData:
        case DestroyStructure: {
            emit(command->shared_from_this());
        } break;

        default:
            break;
    }
}

void GameArbiter::spawn_units() {
    for (int i = 0; i < game->level.height; i++)
        for (int j = 0; j < game->level.width; j++) {
            Point position(j, i);
            Tile& tile = game->level.tiles[position];

            int spawn_group = tile.get_property<int>(SpawnGroup);
            if (!spawn_group)
                continue;

            std::vector<UnitStack::pointer> nearby_stacks;
            int num_nearby = game->get_nearby_stacks(position, 3, nearby_stacks);
            if (num_nearby > 0)
                continue;

            int random100 = rand() % 100;
            int spawn_chance = tile.get_property<int>(SpawnGroup);
            if (random100 > spawn_chance)
                continue;

            int stack_id = game->get_free_stack_id();
            int faction_id = 1;   // Independent

            int spawn_max = (rand() % 8) + 1;
            int spawn_count = 0;
            for (int k = 0; k < spawn_max; k++) {
                for (auto iter = game->unit_types.begin(); iter != game->unit_types.end(); iter++) {
                    const UnitType::pointer type = iter->second;
                    if (type->get_property<int>(SpawnGroup) != spawn_group)
                        continue;

                    random100 = rand() % 100;
                    spawn_chance = type->get_property<int>(SpawnChance);
                    if (random100 > spawn_chance)
                        continue;

                    if (spawn_count == 0) {
                        emit(create_message(CreateStack, stack_id, position, faction_id));
                    }
                    emit(create_message(CreateUnit, stack_id, type->name));
                    spawn_count++;
                }
            }
            if (spawn_count > 0)
                BOOST_LOG_TRIVIAL(debug) << boost::format("Spawned %d units from group %d at %s") % spawn_count % spawn_group % position;
        }
}

void GameArbiter::emit(boost::shared_ptr<Message> update) {
    publisher->receive(update);
    update_counter.receive(update.get());
}
