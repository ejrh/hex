#include "common.h"

#include "hexgame/game/end_of_turn.h"
#include "hexgame/game/game.h"


void EndOfTurn::apply() {
    for (auto iter = game->stacks.begin(); iter != game->stacks.end(); iter++) {
        UnitStack::pointer stack = iter->second;
        for (auto unit_iter = stack->units.begin(); unit_iter != stack->units.end(); unit_iter++) {
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
    int health = unit.properties.get(Health);
    int max_health = unit.type->properties.get(Health);
    if (health > 0 && health < max_health)
        unit.properties.increment(Health, 1);
}
