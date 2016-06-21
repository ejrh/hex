#include "common.h"

#include "hex/game/game.h"
#include "hex/game/combat/combat.h"

Participant::Participant(int id, Side side, int stack_num, UnitStack::pointer stack, int unit_number):
        id(id), side(side), stack_num(stack_num), stack(stack), unit_number(unit_number)
{
    this->unit = stack->units[unit_number]->copy();
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
    if (!is_alive())
        return false;
    return true;
}

bool Participant::is_alive() const {
    return unit->get_property(Health) > 0;
}

int Participant::adjust_health(int change) {
    unit->properties[Health] += change;
    if (unit->properties[Health] < 0)
        unit->properties[Health] = 0;
    else if (unit->properties[Health] > unit->type->properties[Health])
        unit->properties[Health] = unit->type->properties[Health];
    return unit->get_property(Health);
}

std::ostream& operator<<(std::ostream& os, const Participant& p) {
    os << boost::format("[%d %c] S%d/U%d (%s) ") % p.id % ((p.side == Attacker) ? 'A' : 'D') % p.stack->id % p.unit_number % p.unit->type->name << p.unit->properties;
    return os;
}
