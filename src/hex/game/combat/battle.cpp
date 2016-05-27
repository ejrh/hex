#include "common.h"

#include "hex/basics/hexgrid.h"
#include "hex/game/game.h"
#include "hex/game/combat/combat.h"

Battle::Battle(Game *game, const Point& target_point, const Point& attacking_point):
        game(game), target_point(target_point), attacking_point(attacking_point), phase(Charge), turn(0)
{
    set_up_participants();
}

Battle::Battle(Game *game, const Point& target_point, const Point& attacking_point, const std::vector<Move>& moves):
        moves(moves), game(game), target_point(target_point), attacking_point(attacking_point), phase(Charge), turn(0)
{
    set_up_participants();
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
    while (!finished()) {
        step();
    }
}

bool Battle::finished() const {
    int attacking_health = 0;
    int defending_health = 0;
    for (std::vector<Participant>::const_iterator iter = participants.begin(); iter != participants.end(); iter++) {
        const Participant& participant = *iter;
        if (participant.side == Attacker)
            attacking_health += participant.health;
        else if (participant.side == Defender)
            defending_health += participant.health;
    }

    return attacking_health <= 0 || defending_health <= 0;
}

void Battle::step() {
    BOOST_LOG_TRIVIAL(trace) << "Phase " << phase << ", turn " << turn;
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
    int attack_value = rand() % (participant.get_attack() + 1);
    int defence_value = rand() % (target.get_defence() + 1);
    int damage;
    if (attack_value >= defence_value) {
        damage = rand() % (participant.get_damage() + 1);
    } else {
        damage = 0;
    }

    Move move(participant.id, target.id, damage);
    moves.push_back(move);
    apply_move(move);
}


void Battle::apply_move(const Move& move) {
    Participant& participant = participants[move.participant_id];
    Participant& target = participants[move.target_id];
    target.health -= move.damage;
    if (target.health <= 0) {
        BOOST_LOG_TRIVIAL(trace) << "Target death";
        target.health = 0;
    }
}
