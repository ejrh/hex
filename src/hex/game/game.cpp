#include "common.h"

#include "hex/basics/error.h"
#include "hex/game/game.h"


void Game::set_level_data(const Point& offset, const std::vector<std::string>& tile_data) {
    for (unsigned int i = 0; i < tile_data.size(); i++) {
        Point tile_pos(offset.x + i, offset.y);
        if (!level.contains(tile_pos)) {
            std::cerr << "Tile coordinate " << tile_pos << " is outside the level" << std::endl;
            continue;
        }

        std::vector<std::string> parts;
        boost::split(parts, tile_data[i], boost::is_any_of("/"));
        std::string& tile_type_name = parts[0];

        TileType *tile_type = tile_types[tile_type_name];
        Tile& tile = level.tiles[tile_pos];
        tile.type = tile_type;
        tile.road = false;

        for (unsigned int j = 1; j < parts.size(); j++) {
            if (parts[j] == "r") {
                tile.road = true;
            } else if (parts[j][0] == 'm') {
                std::istringstream ss(parts[j]);
                char c;
                int m;
                ss >> c >> m;
                tile.mountain = m;
            }
        }
    }
}

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


Faction *Game::create_faction(int id, const std::string& type_name, const std::string& name) {
    if (factions.find(id) != factions.end()) {
        warn("Faction already exists with id %d", id);
        return NULL;
    }

    Faction *faction = new Faction(id, type_name, name);
    factions[id] = faction;
    return faction;
}

UnitStack *Game::create_unit_stack(int id, const Point position, int owner_id) {
    Faction *owner = get_faction(owner_id);

    if (owner == NULL) {
        warn("No faction with id %d", owner_id);
        return NULL;
    }

    if (level.tiles[position].stack != NULL) {
        warn("Stack already exists at position %d,%d", position.x, position.y);
        return NULL;
    }

    if (stacks.find(id) != stacks.end()) {
        warn("Stack already exists with id %d", id);
        return NULL;
    }

    UnitStack *new_stack = new UnitStack(id, position, owner);
    stacks[id] = new_stack;

    level.tiles[position].stack = new_stack;

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

void Game::destroy_unit_stack(int stack_id) {
    UnitStack *stack = stacks[stack_id];
    stacks.erase(stack_id);
    delete stack;
}

Faction *Game::get_faction(int id) {
    if (factions.find(id) != factions.end())
        return factions[id];
    return NULL;
}

UnitStack *Game::get_stack(int id) {
    if (stacks.find(id) != stacks.end())
        return stacks[id];
    return NULL;
}
