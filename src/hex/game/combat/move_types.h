#ifndef MOVE_TYPES
#define MOVE_TYPES

class Battle;
class Participant;
class Move;

class MoveType {
public:
    MoveType(): type(UnknownPropertyType) { }
    MoveType(PropertyType type): type(type) { }
    virtual int expected_value(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual Move generate(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual void apply(Battle& battle, const Move& move) const;

protected:
    PropertyType type;
};

class ArcheryMoveType: public MoveType {
public:
    ArcheryMoveType(): MoveType(Archery) { };
};

class ChargeMoveType: public MoveType {
public:
    ChargeMoveType(): MoveType(Charge) { };
};

class HealingMoveType: public MoveType {
public:
    HealingMoveType(): MoveType(Healing) { };

    virtual int expected_value(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual Move generate(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual void apply(Battle& battle, const Move& move) const;
};

class StrikeMoveType: public MoveType {
public:
    StrikeMoveType(): MoveType(Strike) { };
};

int damage_roll(const Participant& participant, const Participant& target);

int average_damage_roll(const Participant& participant, const Participant& target, int num_trials = 5);

#endif
