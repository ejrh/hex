#include "common.h"

#include "hex/basics/error.h"
#include "hex/noise.h"

#include "hex/game/game.h"

TileType *Game::create_tile_type(TileType& tile_type) {
    if (tile_types.find(tile_type.name) != tile_types.end()) {
        warn("Tile type already exists: %s", tile_type.name.c_str());
        return NULL;
    }

    TileType *new_tile_type = new TileType(tile_type);

    tile_types[tile_type.name] = new_tile_type;

    return new_tile_type;
}

UnitType *Game::create_unit_type(UnitType& unit_type) {
    if (unit_types.find(unit_type.name) != unit_types.end()) {
        warn("Unit type already exists: %s", unit_type.name.c_str());
        return NULL;
    }

    UnitType *new_unit_type = new UnitType(unit_type);

    unit_types[unit_type.name] = new_unit_type;

    return new_unit_type;
}

UnitStack *Game::create_unit_stack(int id, const Point position) {
    if (level->tiles[position].stack != NULL) {
        warn("Stack already exists at position %d,%d", position.x, position.y);
        return NULL;
    }

    if (stacks.find(id) != stacks.end()) {
        warn("Stack already exists with id %d", id);
        return NULL;
    }

    UnitStack *new_stack = new UnitStack(id, position);
    stacks[id] = new_stack;

    level->tiles[position].stack = new_stack;

    return new_stack;
}

Unit *Game::create_unit(int stack_id, const std::string& type_name) {
    UnitType *type = unit_types[type_name];
    if (type == NULL) {
        warn("No unit type: %s", type_name.c_str());
        return NULL;
    }

    UnitStack *stack = get_stack(stack_id);
    if (stack == NULL) {
        warn("No stack: %d", stack_id);
        return NULL;
    }

    Unit *new_unit = new Unit();
    new_unit->type = type;
    //new_unit->health = type->health;

    stack->units.push_back(new_unit);
    return new_unit;
}

UnitStack *Game::get_stack(int id) {
    if (stacks.find(id) != stacks.end())
        return stacks[id];
    return NULL;
}
