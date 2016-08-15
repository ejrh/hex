#include "common.h"

#include "hex/basics/hexgrid.h"
#include "hex/game/game.h"
#include "hex/game/combat/combat.h"
#include "hex/game/combat/combat_model.h"
#include "hex/game/combat/move_types.h"

Battle::Battle(Game *game, const Point& target_point, const Point& attacking_point):
        game(game), target_point(target_point), attacking_point(attacking_point), turn(0)
{
    set_up_participants();
    combat_model = CombatModel::get_default();
}

Battle::Battle(Game *game, const Point& target_point, const Point& attacking_point, const std::vector<Move>& moves):
        moves(moves), game(game), target_point(target_point), attacking_point(attacking_point), turn(0)
{
    set_up_participants();
    combat_model = CombatModel::get_default();
}

void Battle::set_up_participants() {
    BOOST_LOG_TRIVIAL(trace) << "Setting up battle participants";

    UnitStack::pointer attacker = game->level.tiles[attacking_point].stack;

    for (int dir = 0; dir < 7; dir++) {
        Point stack_point;
        UnitStack::pointer stack;

        get_neighbour(target_point, dir, &stack_point);
        if (game->level.contains(stack_point)) {
            stack = game->level.tiles[stack_point].stack;
        }

        if (stack && stack->owner == attacker->owner) {
            stacks[dir] = stack;
            stack_sides[dir] = Attacker;
        } else if (stack && stack->owner != attacker->owner) {
            stacks[dir] = stack;
            stack_sides[dir] = Defender;
        } else  {
            stacks[dir] = UnitStack::pointer();
            stack_sides[dir] = None;
        }
    }

    for (int dir = 0; dir < 7; dir++) {
        if (stacks[dir]) {
            BOOST_LOG_TRIVIAL(trace) << "Stack " << dir << ": " << stacks[dir]->id;
            for (unsigned int i = 0; i < stacks[dir]->units.size(); i++) {
                int participant_id = participants.size();
                participants.push_back(Participant(participant_id, stack_sides[dir], dir, stacks[dir], i));
                BOOST_LOG_TRIVIAL(trace) << "Participant: " << participants[participant_id];
            }
        }
    }
}

void Battle::run() {
    last_attacking_health = 0;
    last_defending_health = 0;
    rounds_without_injury = 0;
    do {
        step();
    } while (!check_finished());
}

bool Battle::check_finished() {
    int attacking_health = 0;
    int defending_health = 0;
    for (std::vector<Participant>::const_iterator iter = participants.begin(); iter != participants.end(); iter++) {
        const Participant& participant = *iter;
        int health = participant.get_health();
        if (participant.side == Attacker)
            attacking_health += health;
        else if (participant.side == Defender)
            defending_health += health;
    }

    bool no_injury = last_attacking_health == attacking_health && last_defending_health == defending_health;
    if (no_injury)
        rounds_without_injury++;
    else
        rounds_without_injury = 0;
    bool finished = attacking_health <= 0 || defending_health <= 0 || rounds_without_injury >= 3;
    BOOST_LOG_TRIVIAL(trace) << boost::format("Finished: %s; with attacking %d (was %d), defending %d (was %d)") % finished % attacking_health % last_attacking_health % defending_health % last_defending_health;
    last_attacking_health = attacking_health;
    last_defending_health = defending_health;
    return finished;
}

void Battle::step() {
    BOOST_LOG_TRIVIAL(trace) << "Turn " << turn;
    for (std::vector<Participant>::iterator iter = participants.begin(); iter != participants.end(); iter++) {
        step_participant(*iter);
    }
    turn++;
}

void Battle::step_participant(Participant& participant) {
    if (!participant.can_move())
        return;

    BOOST_LOG_TRIVIAL(trace) << "Participant " << participant;
    make_move(participant);
}

void Battle::make_move(Participant& participant) {
    std::vector<const MoveType *> move_types = combat_model->get_available_move_types(*this, participant);

    const MoveType *best = NULL;
    int best_target = -1;
    float best_value = 0;
    int num_considered = 0;
    for (std::vector<Participant>::const_iterator iter = participants.begin(); iter != participants.end(); iter++) {
        const Participant& target = *iter;
        for (std::vector<const MoveType *>::const_iterator iter2 = move_types.begin(); iter2 != move_types.end(); iter2++) {
            const MoveType *type = *iter2;
            if (!type->is_viable(*this, participant, target))
                continue;

            num_considered++;

            float expected_value = type->expected_value(*this, participant, target) * type->repeats();

            if (expected_value > best_value) {
                best = type;
                best_target = target.id;
                best_value = expected_value;
            }
        }
    }

    if (best != NULL && best_target != -1) {
        BOOST_LOG_TRIVIAL(trace) << boost::format("Considered %d moves and chose: ") % num_considered << *best << boost::format(" (with value %0.1f)") % best_value;
        Participant& target = participants[best_target];
        for (int i = 0; i < best->repeats(); i++) {
            // It's possible the move is no longer viable
            if (!best->is_viable(*this, participant, target))
                break;

            // Make the move
            Move move = best->generate(*this, participant, target);
            moves.push_back(move);
            apply_move(move);

            // Riposte
            MoveType *riposte_type = combat_model->move_types[Riposte];
            if (riposte_type != NULL && riposte_type->is_viable(*this, target, participant)) {
                Move riposte = riposte_type->generate(*this, target, participant);
                moves.push_back(riposte);
                apply_move(riposte);
            }
        }
    } else {
        BOOST_LOG_TRIVIAL(trace) << "Considered no moves";
    }
}

void Battle::apply_move(const Move& move) {
    BOOST_LOG_TRIVIAL(trace) << "Move: " << move;
    const MoveType *move_type = combat_model->get_move_type(move);
    move_type->apply(*this, move);
    Participant& target = participants[move.target_id];
    if (target.get_health() <= 0) {
        BOOST_LOG_TRIVIAL(trace) << "Target death";
        target.unit->properties[Health] = 0;
    }
}

void Battle::replay() {
    for (std::vector<Move>::iterator iter = moves.begin(); iter != moves.end(); iter++) {
        Move& move = *iter;
        replay_move(move);
    }
}

void Battle::replay_move(const Move& move) {
    apply_move(move);
}

void Battle::commit() {
    BOOST_LOG_TRIVIAL(trace) << "Committing battle results";

    for (std::vector<Participant>::const_iterator iter = participants.begin(); iter != participants.end(); iter++) {
        const Participant& participant = *iter;

        participant.stack->units[participant.unit_number]->type = participant.unit->type;
        participant.stack->units[participant.unit_number]->properties = participant.unit->properties;
    }

    for (int dir = 0; dir < 7; dir++) {
        if (stacks[dir]) {
            UnitStack& stack = *stacks[dir];
            BOOST_LOG_TRIVIAL(trace) << "Stack " << dir << ": " << stack.id;
            std::vector<Unit::pointer>::iterator iter = stack.units.begin();
            while (iter != stack.units.end()) {
                Unit& unit = **iter;
                if (unit.get_property<int>(Health) <= 0) {
                    BOOST_LOG_TRIVIAL(trace) << "Unit: " << unit.type->name << " (dead)";
                    iter = stack.units.erase(iter);
                } else {
                    BOOST_LOG_TRIVIAL(trace) << "Unit: " << unit.type->name << " " << unit.properties;
                    iter++;
                }
            }
            if (stack.units.size() == 0) {
                BOOST_LOG_TRIVIAL(trace) << "Stack was destroyed";
                game->destroy_unit_stack(stack.id);
            }
        }
    }
}
