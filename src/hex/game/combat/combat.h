#ifndef BATTLE_H
#define BATTLE_H

#include "move.h"

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

class Participant {
public:
    Participant(int id, Side side, int stack_num, UnitStack::pointer stack, int unit_number);
    int get_attack() const;
    int get_defence() const;
    int get_damage() const;
    bool can_move() const;
    bool can_attack(const Participant& other) const;
    bool can_heal(const Participant& other) const;

public:
    int id;
    Side side;

    int stack_num;
    UnitStack::pointer stack;
    int unit_number;
    Unit::pointer unit;

    int health;
};

std::ostream& operator<<(std::ostream& os, const Participant& p);


class Battle {
public:
    Battle(Game *game, const Point& target_point, const Point& attacking_point);
    Battle(Game *game, const Point& target_point, const Point& attacking_point, const std::vector<Move>& moves);

    void set_up_participants();
    void run();
    bool finished() const;
    void step();
    void step_participant(Participant& participant);
    void make_move(Participant& participant, Participant& target);
    void apply_move(const Move& move);

public:
    std::vector<Participant> participants;
    std::vector<Move> moves;

    Game *game;
    Point target_point;
    Point attacking_point;
    UnitStack::pointer stacks[7];
    Side stack_sides[7];
    BattlePhase phase;
    int turn;
};

#endif
