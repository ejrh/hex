#include "common.h"

#include "hexutil/basics/properties.h"

#include "hexgame/game/game.h"
#include "hexgame/game/combat/combat.h"
#include "hexgame/game/combat/move.h"
#include "hexgame/game/combat/move_types.h"


namespace hex {

int damage_roll(int attack, int defence, int damage) {
    if (attack < 1)
        attack = 1;
    if (defence < 1)
        defence = 1;
    if (damage < 1)
        damage = 1;
    int attack_value = rand() % (attack + 1);
    int defence_value = rand() % (defence + 1);
    int damage_value;
    if (attack_value > defence_value) {
        damage_value = rand() % (damage + 1);
    } else {
        damage_value = 0;
    }
    return damage_value;
}

int damage_roll(const Participant& participant, const Participant& target) {
    return damage_roll(participant.get_attack(), target.get_defence(), participant.get_damage());
}

float average_damage_roll(const Participant& participant, const Participant& target, int num_trials) {
    float total_damage = 0;
    for (int i = 0; i < num_trials; i++) {
        total_damage += damage_roll(participant, target);
    }
    return total_damage / num_trials;
}

bool MoveType::is_viable(const Battle& battle, const Participant& participant, const Participant& target) const {
    if (!participant.can_move())
        return false;

    if (direction == Beneficial)
        return participant.side == target.side && target.is_alive();
    else if (direction == Detrimental)
        return participant.side != target.side && target.is_alive();
    else
        return true;
}

float MoveType::expected_value(const Battle& battle, const Participant& participant, const Participant& target) const {
    return average_damage_roll(participant, target);
}

int MoveType::repeats() const {
    return num_repeats;
}

Move MoveType::generate(const Battle& battle, const Participant& participant, const Participant& target) const {
    Move move;
    move.participant_id = participant.id;
    move.target_id = target.id;
    move.type = type;
    move.effect = damage_roll(participant, target);
    return move;
}

void MoveType::apply(Battle& battle, const Move& move) const {
    Participant& target = battle.participants[move.target_id];
    target.adjust_health(-move.effect);
}

bool ChargeMoveType::is_viable(const Battle& battle, const Participant& participant, const Participant& target) const {
    return battle.turn == 0 && MoveType::is_viable(battle, participant, target);
}

float ChargeMoveType::expected_value(const Battle& battle, const Participant& participant, const Participant& target) const {
    return MoveType::expected_value(battle, participant, target) + 2;
}

Move ChargeMoveType::generate(const Battle& battle, const Participant& participant, const Participant& target) const {
    assert(battle.turn == 0);
    Move move;
    move.participant_id = participant.id;
    move.target_id = target.id;
    move.type = type;
    move.effect = damage_roll(participant.get_attack() + 2, target.get_defence(), participant.get_attack());
    return move;
}

bool HealingMoveType::is_viable(const Battle& battle, const Participant& participant, const Participant& target) const {
    return participant.id != target.id && MoveType::is_viable(battle, participant, target);
}

float HealingMoveType::expected_value(const Battle& battle, const Participant& participant, const Participant& target) const {
    return std::min((float) target.unit->type->get_property<int>(Health) - target.unit->get_property<int>(Health), 5.0f);
}

Move HealingMoveType::generate(const Battle& battle, const Participant& participant, const Participant& target) const {
    Move move;
    move.participant_id = participant.id;
    move.target_id = target.id;
    move.type = Healing;
    move.effect = 5;
    return move;
}

void HealingMoveType::apply(Battle& battle, const Move& move) const {
    Participant& participant = battle.participants[move.participant_id];
    participant.unit->properties.set<int>(Healing, 0);

    Participant& target = battle.participants[move.target_id];
    target.adjust_health(move.effect);
}

bool RiposteMoveType::is_viable(const Battle& battle, const Participant& participant, const Participant& target) const {
    if (!MoveType::is_viable(battle, participant, target))
        return false;
    if (battle.moves.empty())
        return false;
    const Move& last_move = battle.moves[battle.moves.size() - 1];
    return last_move.type == Strike && participant.unit->has_property(Strike);
}

Move RiposteMoveType::generate(const Battle& battle, const Participant& participant, const Participant& target) const {
    Move move;
    move.participant_id = participant.id;
    move.target_id = target.id;
    move.type = type;
    move.effect = damage_roll(participant.get_attack() - 1, target.get_defence(), participant.get_attack() - 1);
    return move;
}

};
