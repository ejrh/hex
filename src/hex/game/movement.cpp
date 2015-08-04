#include "common.h"

#include "hex/game/game.h"
#include "hex/game/movement.h"
#include "hex/game/traits.h"


int MovementModel::cost_to(const UnitStack *party, const Tile *tile) const {
    if (party->units.size() == 0)
        return INT_MAX;

    UnitStack *target_stack = tile->stack;
    if (target_stack != NULL) {
        if (target_stack->owner != party->owner)
            return INT_MAX;
    }

    // TODO use all members of party to determine cost
    Unit *unit = *party->units.begin();

    if (tile->has_property(Walkable) && unit->has_ability(Walking)) {
        return 30;
    }

    if (tile->has_property(Swimmable) && unit->has_ability(Swimming)) {
        return 40;
    }

    if (tile->has_property(Flyable) && unit->has_ability(Flying)) {
        return 40;
    }

    return INT_MAX;
}

int MovementModel::admits(const UnitType *unit_type, const TileType *tile_type) const {
    if (unit_type->has_ability(Walking) && tile_type->has_property(Walkable))
        return true;
    else if (unit_type->has_ability(Swimming) && tile_type->has_property(Swimmable))
        return true;
    if (unit_type->has_ability(Flying) && tile_type->has_property(Flyable))
        return true;
    return false;
}
