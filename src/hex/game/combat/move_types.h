#ifndef MOVE_TYPES
#define MOVE_TYPES

class Battle;
class Participant;
class Move;

enum MoveDirection {
    UnknownMoveDirection,
    Beneficial,
    Detrimental
};

class MoveType {
public:
    MoveType(): type(UnknownPropertyType), direction(UnknownMoveDirection) { }
    MoveType(PropertyType type, MoveDirection direction): type(type), direction(direction) { }
    virtual ~MoveType() { }

    virtual bool is_viable(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual float expected_value(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual Move generate(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual void apply(Battle& battle, const Move& move) const;

protected:
    PropertyType type;
    MoveDirection direction;
};

class ArcheryMoveType: public MoveType {
public:
    ArcheryMoveType(): MoveType(Archery, Detrimental) { };
};

class ChargeMoveType: public MoveType {
public:
    ChargeMoveType(): MoveType(Charge, Detrimental) { };
    virtual bool is_viable(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual float expected_value(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual Move generate(const Battle& battle, const Participant& participant, const Participant& target) const;
};

class HealingMoveType: public MoveType {
public:
    HealingMoveType(): MoveType(Healing, Beneficial) { };
    virtual bool is_viable(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual float expected_value(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual Move generate(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual void apply(Battle& battle, const Move& move) const;
};

class StrikeMoveType: public MoveType {
public:
    StrikeMoveType(): MoveType(Strike, Detrimental) { };
};

int damage_roll(const Participant& participant, const Participant& target);

float average_damage_roll(const Participant& participant, const Participant& target, int num_trials = 5);

#endif
