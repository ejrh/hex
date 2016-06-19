#include "common.h"

#include "hex/game/game.h"
#include "hex/game/properties.h"
#include "hex/game/combat/combat.h"
#include "hex/game/combat/move.h"
#include "hex/game/combat/move_types.h"

int damage_roll(const Participant& participant, const Participant& target) {
    int attack_value = rand() % (participant.get_attack() + 1);
    int defence_value = rand() % (target.get_defence() + 1);
    int damage;
    if (attack_value > defence_value) {
        damage = rand() % (participant.get_damage() + 1);
    } else {
        damage = 0;
    }

    return damage;
}

int average_damage_roll(const Participant& participant, const Participant& target, int num_trials) {
    int total_damage = 0;
    for (int i = 0; i < num_trials; i++) {
        total_damage += damage_roll(participant, target);
    }
    return total_damage / num_trials;
}

int MoveType::expected_value(const Battle& battle, const Participant& participant, const Participant& target) const {
    return average_damage_roll(participant, target);
};

Move MoveType::generate(const Battle& battle, const Participant& participant, const Participant& target) const {
    Move move;
    move.participant_id = participant.id;
    move.target_id = target.id;
    move.type = type;
    move.damage = damage_roll(participant, target);
    return move;
};

void MoveType::apply(Battle& battle, const Move& move) const {
    Participant& participant = battle.participants[move.participant_id];
    Participant& target = battle.participants[move.target_id];
    target.health -= move.damage;
    if (target.health <= 0) {
        BOOST_LOG_TRIVIAL(trace) << "Target death";
        target.health = 0;
    }
};

int HealingMoveType::expected_value(const Battle& battle, const Participant& participant, const Participant& target) const {
    return 5;
};

Move HealingMoveType::generate(const Battle& battle, const Participant& participant, const Participant& target) const {
    Move move;
    move.participant_id = participant.id;
    move.target_id = target.id;
    move.type = Healing;
    move.damage = 0;
    return move;
};

void HealingMoveType::apply(Battle& battle, const Move& move) const {
    Participant& target = battle.participants[move.target_id];
    target.health += 5;
    if (target.health >= target.max_health) {
        target.health = target.max_health;
    }
};
