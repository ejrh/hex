#include "common.h"

#include "hex/basics/error.h"
#include "hex/game/game.h"
#include "hex/game/movement/movement.h"
#include "hex/game/traits.h"


MovementModel::MovementModel(Level *level): level(level), target_pos(-1, -1) { }

int MovementModel::cost_to(const UnitStack *party, const Point& tile_pos) const {
    if (party->units.size() == 0)
        return INT_MAX;

    Tile &tile = level->tiles[tile_pos];

    UnitStack *target_stack = tile.stack;
    if (target_stack != NULL) {
        // Can't move through enemy stacks
        if (tile_pos != target_pos && target_stack->owner != party->owner)
            return INT_MAX;
    }

    //TODO if one unit is a transport, then the others move for free

    int total_cost = 0;
    for (std::vector<Unit *>::const_iterator iter = party->units.begin(); iter != party->units.end(); iter++) {
        Unit *unit = *iter;
        int cost = cost_to(unit, tile_pos);
        if (cost == INT_MAX) {
            total_cost = INT_MAX;
            break;
        }
        total_cost += cost;
    }

    return total_cost;
}

int MovementModel::cost_to(const Unit *unit, const Point& tile_pos) const {
    Tile &tile = level->tiles[tile_pos];

    if (tile.has_property(Walkable) && unit->has_ability(Walking)) {
        if (tile.road)
            return 3;
        else if (tile.has_property(SlowWalking))
            return 5;
        else
            return 4;
    }

    if (tile.has_property(Swimmable) && unit->has_ability(Swimming)) {
        return 4;
    }

    if (tile.has_property(Flyable) && unit->has_ability(Flying)) {
        if (tile.has_property(SlowFlying))
            return 5;
        else
            return 4;
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

void MovementModel::move(UnitStack *party, const Point& tile_pos) const {
    for (std::vector<Unit *>::iterator iter = party->units.begin(); iter != party->units.end(); iter++) {
        Unit *unit = *iter;
        int cost = cost_to(unit, tile_pos);
        unit->moves -= cost;
    }
}

int MovementModel::check_path(const UnitStack *stack, const Path& path) const {
    StackMovePoints points(stack);
    unsigned int step_num = 0;
    while (step_num < path.size()) {
        if (!check_step(stack, &points, path, step_num))
            break;
        step_num++;
    }
    return step_num;
}

bool MovementModel::check_step(const UnitStack *stack, StackMovePoints *points, const Path& path, int step_num) const {
    if (stack->units.size() == 0)
        return false;

    Point next_step = path[step_num];
    for (unsigned int i = 0; i < stack->units.size(); i++) {
        Unit *unit = stack->units[i];
        int cost = cost_to(unit, next_step);
        points->points[i] -= cost;
    }

    return !points->exhausted();
}
