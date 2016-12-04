#include "common.h"

#include "hexgame/game/game.h"
#include "hexgame/game/combat/combat.h"

Participant::Participant(int id, Side side, int stack_num, UnitStack::pointer stack, int unit_number):
        id(id), side(side), stack_num(stack_num), stack(stack), unit_number(unit_number)
{
    this->unit = stack->units[unit_number]->copy();
}

int Participant::get_attack() const {
    return unit->get_property<int>(Attack);
}

int Participant::get_defence() const {
    return unit->get_property<int>(Defence);
}

int Participant::get_damage() const {
    return unit->get_property<int>(Damage);
}

bool Participant::can_move() const {
    if (!is_alive())
        return false;
    return true;
}

int Participant::get_health() const {
    return unit->get_property<int>(Health);
}

int Participant::get_max_health() const {
    return unit->type->get_property<int>(Health);
}

bool Participant::is_alive() const {
    return unit->get_property<int>(Health) > 0;
}

int Participant::adjust_health(int change) {
    unit->properties.increment(Health, change);
    if (unit->get_property<int>(Health) < 0)
        unit->set_property<int>(Health, 0);
    else if (unit->get_property<int>(Health) > unit->type->get_property<int>(Health))
        unit->set_property(Health, unit->type->get_property<int>(Health));
    return unit->get_property<int>(Health);
}

std::ostream& operator<<(std::ostream& os, const Participant& p) {
    os << boost::format("[%d %c] S%d/U%d (%s) ") % p.id % ((p.side == Attacker) ? 'A' : 'D') % p.stack->id % p.unit_number % p.unit->type->name << p.unit->properties;
    return os;
}
