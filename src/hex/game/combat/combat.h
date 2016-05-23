#ifndef BATTLE_H
#define BATTLE_H

class UnitStack;
class Unit;

enum BattlePhase {
    Charge,
    Melee
};

enum Side {
    None,
    Attacker,
    Defender
};

class Move {
public:
    Move(int participant_id, int target_id, int damage):
        participant_id(participant_id), target_id(target_id), damage(damage) { }

public:
    int participant_id;
    int target_id;
    int damage;
};

inline std::ostream& operator<<(std::ostream& os, const Move& a) {
    os << boost::format("%d-%d, damage %d") % a.participant_id % a.target_id % a.damage;
    return os;
}

class Participant {
public:
    Participant(int id, Side side, UnitStack::pointer stack, int unit_number);
    int get_attack() const;
    int get_defence() const;
    int get_damage() const;
    bool can_move() const;
    bool can_attack(const Participant& other) const;
    bool can_heal(const Participant& other) const;

public:
    int id;
    Side side;

    UnitStack::pointer stack;
    int unit_number;
    Unit::pointer unit;

    int health;
};

std::ostream& operator<<(std::ostream& os, const Participant& p);


class Battle {
public:
    Battle(Game *game, const Point& target_point, const Point& attacking_point);

    void set_up_participants();
    void run();
    bool finished() const;
    void step();
    void step_participant(Participant& participant);
    void make_move(Participant& participant, Participant& target);
    void apply_move(const Move& move);

public:
    std::vector<Move> moves;

private:
    Game *game;
    Point target_point;
    Point attacking_point;
    UnitStack::pointer stacks[7];
    Side stack_sides[7];
    std::vector<Participant> participants;
    BattlePhase phase;
    int turn;
};

#endif
