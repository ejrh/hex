#include "common.h"

#include "hex/game/end_of_turn.h"
#include "hex/game/game.h"


void EndOfTurn::apply() {
    for (IntMap<UnitStack>::iterator iter = game->stacks.begin(); iter != game->stacks.end(); iter++) {
        UnitStack::pointer stack = iter->second;
        for (std::vector<Unit::pointer>::iterator unit_iter = stack->units.begin(); unit_iter != stack->units.end(); unit_iter++) {
            Unit& unit = **unit_iter;
            restore_moves(unit);
            restore_health(unit);
        }
    }
}

void EndOfTurn::restore_moves(Unit& unit) {
    unit.properties[Moves] = unit.type->properties[Moves];
}

void EndOfTurn::restore_health(Unit& unit) {
    if (unit.properties[Health] > 0 && unit.properties[Health] < unit.type->properties[Health])
        unit.properties[Health]++;
}