#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/basics/hexgrid.h"

#include "hexgame/game/game.h"
#include "hexgame/game/movement/movement.h"


namespace hex {

MovementModel::MovementModel(Game *game): game(game), target_pos(-1, -1) { }

MovementModel::MovementModel(Game *game, const Point& target_pos): game(game), target_pos(target_pos) { }

bool MovementModel::can_enter(const UnitStack& party, const Point& tile_pos) const {
    if (party.units.size() == 0)
        return false;

    Tile &tile = game->level.tiles[tile_pos];

    UnitStack::pointer target_stack = tile.stack;
    if (target_stack) {
        // Can't move through enemy stacks
        if (tile_pos != target_pos && target_stack->owner != party.owner)
            return false;

        //TODO can't move through a structure defended by a neighbouring enemy, either

        if (target_stack->units.size() + party.units.size() > MAX_UNITS)
            return false;

        //TODO make sure there will not be two transports in the new stack
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
    Tile &tile = game->level.tiles[tile_pos];

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

int MovementModel::admits(const UnitType& unit_type, const Tile& tile) const {
    if (unit_type.has_property(Walking) && tile.has_property(Walkable))
        return true;
    else if (unit_type.has_property(Swimming) && tile.has_property(Swimmable))
        return true;
    else if (unit_type.has_property(Sailing) && tile.has_property(Sailable))
        return true;
    if (unit_type.has_property(Flying) && tile.has_property(Flyable))
        return true;
    return false;
}

void MovementModel::move(UnitStack& party, const IntSet& selected_units, const Point& tile_pos) const {
    for (unsigned int i = 0; i < party.units.size(); i++) {
        if (selected_units.contains(i)) {
            Unit& unit = *party.units[i];
            int cost = cost_to(unit, tile_pos);
            unit.properties.increment<int>(Moves, -cost);

            //TODO if unit is on transport, does it affect the terrain?
            if (unit.has_property(PathOfLife)) {
                alter_terrain(tile_pos, PathOfLife);
            } else if (unit.has_property(PathOfDeath)) {
                alter_terrain(tile_pos, PathOfDeath);
            } else if (unit.has_property(PathOfIce)) {
                alter_terrain(tile_pos, PathOfIce);
            }

            Tile& tile = game->level.tiles[tile_pos];
            if (tile.structure) {
                Structure& structure = *tile.structure;
                if (structure.has_property(Capturable) && structure.owner != party.owner) {
                    structure.owner = party.owner;
                }
            }
        }
    }
}

void MovementModel::alter_terrain(Point pos, Atom type) const {
    std::vector<Point> points = get_circle_points(pos, 1, game->level.width, game->level.height);
    TileType::pointer new_tile_type;
    TileType::pointer new_water_tile_type;
    if (type == PathOfLife)
        new_tile_type = game->tile_types.get("grass");
    else if (type == PathOfDeath)
        new_tile_type = game->tile_types.get("wasteland");
    else {
        new_tile_type = game->tile_types.get("snow");
        new_water_tile_type = game->tile_types.get("ice");
        //TODO ice is only temporary!  Maybe we need a terrain type for "temporary ice"
    }
    for (auto iter = points.begin(); iter != points.end(); iter++) {
        if (!game->level.contains(*iter))
            continue;
        Tile& tile = game->level.tiles[*iter];
        if (tile.type->name == "grass" || tile.type->name == "desert" || tile.type->name == "snow" || tile.type->name == "steppe")
            tile.type = new_tile_type;
        else if (tile.type->name == "water" && new_water_tile_type)
            tile.type = new_water_tile_type;
    }
}

int MovementModel::check_path(const UnitStack& stack, const Path& path) const {
    StackMovePoints points(stack);
    unsigned int step_num = 0;
    Point current_position = stack.position;
    while (step_num < path.size()) {
        Point next_position = path[step_num];
        if (distance_between(current_position, next_position) != 1)
            break;
        if (!check_step(stack, &points, path, step_num))
            break;
        current_position = next_position;
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

};
