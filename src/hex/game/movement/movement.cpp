#include "common.h"

#include "hexutil/basics/error.h"

#include "hex/game/game.h"
#include "hex/game/movement/movement.h"


MovementModel::MovementModel(Level *level): level(level), target_pos(-1, -1) { }

MovementModel::MovementModel(Level *level, const Point& target_pos): level(level), target_pos(target_pos) { }

bool MovementModel::can_enter(const UnitStack& party, const Point& tile_pos) const {
    if (party.units.size() == 0)
        return false;

    Tile &tile = level->tiles[tile_pos];

    UnitStack::pointer target_stack = tile.stack;
    if (target_stack) {
        // Can't move through enemy stacks
        if (tile_pos != target_pos && target_stack->owner != party.owner)
            return false;
    }

    return true;
}

int MovementModel::cost_to(const UnitStack& party, const Point& tile_pos) const {
    if (!can_enter(party, tile_pos))
        return INT_MAX;

    //TODO if one unit is a transport, then the others move for free

    int total_cost = 0;
    for (auto iter = party.units.begin(); iter != party.units.end(); iter++) {
        Unit& unit = **iter;
        int cost = cost_to(unit, tile_pos);
        if (cost == INT_MAX) {
            total_cost = INT_MAX;
            break;
        }
        total_cost += cost;
    }

    return total_cost;
}

int MovementModel::cost_to(const Unit& unit, const Point& tile_pos) const {
    Tile &tile = level->tiles[tile_pos];

    if (tile.has_property(Walkable) && unit.has_property(Walking)) {
        if (tile.has_property(Road))
            return 3;
        else if (tile.has_property(SlowWalking))
            return 5;
        else
            return 4;
    }

    if (tile.has_property(Swimmable) && unit.has_property(Swimming)) {
        return 4;
    }

    if (tile.has_property(Sailable) && unit.has_property(Sailing)) {
        return 4;
    }

    if (tile.has_property(Forest) && unit.has_property(Forestry)) {
        return 3;
    }

    if (tile.has_property(Flyable) && unit.has_property(Flying)) {
        if (tile.has_property(SlowFlying))
            return 5;
        else
            return 4;
    }

    return INT_MAX;
}

int MovementModel::admits(const UnitType& unit_type, const TileType& tile_type) const {
    if (unit_type.has_property(Walking) && tile_type.has_property(Walkable))
        return true;
    else if (unit_type.has_property(Swimming) && tile_type.has_property(Swimmable))
        return true;
    else if (unit_type.has_property(Sailing) && tile_type.has_property(Sailable))
        return true;
    if (unit_type.has_property(Flying) && tile_type.has_property(Flyable))
        return true;
    return false;
}

void MovementModel::move(UnitStack& party, const IntSet& selected_units, const Point& tile_pos) const {
    for (unsigned int i = 0; i < party.units.size(); i++) {
        if (selected_units.contains(i)) {
            Unit& unit = *party.units[i];
            int cost = cost_to(unit, tile_pos);
            unit.properties.increment<int>(Moves, -cost);
        }
    }
}

int MovementModel::check_path(const UnitStack& stack, const Path& path) const {
    StackMovePoints points(stack);
    unsigned int step_num = 0;
    while (step_num < path.size()) {
        if (!check_step(stack, &points, path, step_num))
            break;
        step_num++;
    }
    return step_num;
}

bool MovementModel::check_step(const UnitStack& stack, StackMovePoints *points, const Path& path, int step_num) const {
    Point next_step = path[step_num];

    if (!can_enter(stack, next_step))
        return false;

    for (unsigned int i = 0; i < stack.units.size(); i++) {
        Unit& unit = *stack.units[i];
        int cost = cost_to(unit, next_step);
        points->points[i] -= cost;
    }

    return !points->exhausted();
}
