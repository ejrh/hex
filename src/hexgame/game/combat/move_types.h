#ifndef MOVE_TYPES
#define MOVE_TYPES

class Battle;
class Participant;
class Move;

enum MoveDirection {
    UnknownMoveDirection,
    Beneficial,
    Detrimental,
    Returned
};

class MoveType {
public:
    MoveType(): type(0), direction(UnknownMoveDirection), num_repeats(1) { }
    MoveType(Atom type, MoveDirection direction, int num_repeats = 1): type(type), direction(direction), num_repeats(num_repeats) { }
    virtual ~MoveType() { }

    virtual bool is_viable(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual float expected_value(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual int repeats() const;
    virtual Move generate(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual void apply(Battle& battle, const Move& move) const;

public:
    Atom type;
    MoveDirection direction;

protected:
    int num_repeats;

    friend std::ostream& operator<<(std::ostream& os, const MoveType& t);
};

class ArcheryMoveType: public MoveType {
public:
    ArcheryMoveType(): MoveType(Archery, Detrimental, 2) { };
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
    StrikeMoveType(): MoveType(Strike, Detrimental, 2) { };
};

class RiposteMoveType: public MoveType {
public:
    RiposteMoveType(): MoveType(Riposte, Returned) { };
    virtual bool is_viable(const Battle& battle, const Participant& participant, const Participant& target) const;
    virtual Move generate(const Battle& battle, const Participant& participant, const Participant& target) const;
};

int damage_roll(const Participant& participant, const Participant& target);

float average_damage_roll(const Participant& participant, const Participant& target, int num_trials = 5);

inline std::ostream& operator<<(std::ostream& os, const MoveType& t) {
    os << t.type;
    return os;
}

#endif
