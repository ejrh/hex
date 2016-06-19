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
        if (participant.side == Attacker)
            attacking_health += participant.health;
        else if (participant.side == Defender)
            defending_health += participant.health;
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

    int target_id = -1;
    for (std::vector<Participant>::const_iterator iter = participants.begin(); iter != participants.end(); iter++) {
        const Participant& other = *iter;
        if (participant.can_attack(other)) {
            target_id = other.id;
            break;
        }
    }

    if (target_id == -1) {
        BOOST_LOG_TRIVIAL(trace) << "No target";
        return;
    }

    Participant& target = participants[target_id];
    BOOST_LOG_TRIVIAL(trace) << "Target " << target;
    make_move(participant, target);
}

void Battle::make_move(Participant& participant, Participant& target) {
    std::vector<const MoveType *> move_types = combat_model->get_available_move_types(*this, participant);

    const MoveType *best = NULL;
    int best_value = 0;
    for (std::vector<const MoveType *>::const_iterator iter = move_types.begin(); iter != move_types.end(); iter++) {
        if (best == NULL || (*iter)->expected_value(*this, participant, target) > best_value) {
            best = *iter;
            best_value = best->expected_value(*this, participant, target);
        }
    }

    if (best != NULL) {
        Move move = best->generate(*this, participant, target);
        moves.push_back(move);
        apply_move(move);
    }
}

void Battle::apply_move(const Move& move) {
    const MoveType& move_type = combat_model->get_move_type(move);
    move_type.apply(*this, move);
    BOOST_LOG_TRIVIAL(trace) << move;
}
