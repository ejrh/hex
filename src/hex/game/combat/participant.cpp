#include "common.h"

#include "hex/game/game.h"
#include "hex/game/combat/combat.h"

Participant::Participant(int id, Side side, int stack_num, UnitStack::pointer stack, int unit_number):
        id(id), side(side), stack_num(stack_num), stack(stack), unit_number(unit_number)
{
    this->unit = stack->units[unit_number];

    health = unit->get_property(Health);
    max_health = unit->type->get_property(Health);
}

int Participant::get_attack() const {
    return unit->get_property(Attack);
}

int Participant::get_defence() const {
    return unit->get_property(Defence);
}

int Participant::get_damage() const {
    return unit->get_property(Damage);
}

bool Participant::can_move() const {
    if (health <= 0)
        return false;
    return true;
}

bool Participant::can_attack(const Participant& other) const {
    if (other.health <= 0)
        return false;
    return side != other.side;
}

bool Participant::can_heal(const Participant& other) const {
    if (other.health <= 0)
        return false;
    return side == other.side;
}

bool Participant::is_alive() const {
    return health > 0;
}

std::ostream& operator<<(std::ostream& os, const Participant& p) {
    os << boost::format("[%d %c] S%d/U%d (%s) H%d") % p.id % ((p.side == Attacker) ? 'A' : 'D') % p.stack->id % p.unit_number % p.unit->type->name % p.health;
    return os;
}
