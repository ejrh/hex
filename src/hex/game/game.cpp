#include "common.h"

#include "hex/basics/error.h"
#include "hex/game/game.h"
#include "hex/game/movement/movement.h"


void Game::set_level_data(const Point& offset, const std::vector<std::string>& tile_data) {
    for (unsigned int i = 0; i < tile_data.size(); i++) {
        Point tile_pos(offset.x + i, offset.y);
        if (!level.contains(tile_pos)) {
            BOOST_LOG_TRIVIAL(warning) << "Tile coordinate " << tile_pos << " is outside the level";
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
            }
        }
    }
}

TileType *Game::create_tile_type(TileType& tile_type) {
    if (tile_types.find(tile_type.name) != tile_types.end()) {
        BOOST_LOG_TRIVIAL(warning) << "Tile type already exists: " << tile_type.name;
        return NULL;
    }

    TileType *new_tile_type = new TileType(tile_type);

    tile_types[tile_type.name] = new_tile_type;

    return new_tile_type;
}

UnitType *Game::create_unit_type(UnitType& unit_type) {
    if (unit_types.find(unit_type.name) != unit_types.end()) {
        BOOST_LOG_TRIVIAL(warning) << "Unit type already exists: " << unit_type.name;
        return NULL;
    }

    UnitType *new_unit_type = new UnitType(unit_type);

    unit_types[unit_type.name] = new_unit_type;

    return new_unit_type;
}

StructureType *Game::create_structure_type(StructureType& structure_type) {
    if (structure_types.find(structure_type.name) != structure_types.end()) {
        BOOST_LOG_TRIVIAL(warning) << "Structure type already exists: " << structure_type.name;
        return NULL;
    }

    StructureType *new_structure_type = new StructureType(structure_type);
    structure_types[structure_type.name] = new_structure_type;
    return new_structure_type;
}

Faction *Game::create_faction(int id, const std::string& type_name, const std::string& name) {
    if (factions.find(id) != factions.end()) {
        BOOST_LOG_TRIVIAL(warning) << "Faction already exists with id " << id;
        return NULL;
    }

    Faction *faction = new Faction(id, type_name, name);
    factions[id] = faction;
    return faction;
}

UnitStack *Game::create_unit_stack(int id, const Point position, int owner_id) {
    Faction *owner = get_faction(owner_id);

    if (owner == NULL) {
        BOOST_LOG_TRIVIAL(warning) << "No faction with id " << owner_id;
        return NULL;
    }

    if (level.tiles[position].stack != NULL) {
        BOOST_LOG_TRIVIAL(warning) << "Stack already exists at: " << position;
        return NULL;
    }

    if (stacks.find(id) != stacks.end()) {
        BOOST_LOG_TRIVIAL(warning) << "Stack already exists with id " << id;
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
        BOOST_LOG_TRIVIAL(warning) << "No unit type: " << type_name;
        return NULL;
    }

    UnitStack *stack = get_stack(stack_id);
    if (stack == NULL) {
        BOOST_LOG_TRIVIAL(warning) << "No stack: " << stack_id;
        return NULL;
    }

    Unit *new_unit = new Unit();
    new_unit->type = type;
    //new_unit->health = type->health;

    stack->units.push_back(new_unit);
    return new_unit;
}

Structure *Game::create_structure(const Point& position, const std::string& type_name, int owner_id) {
    Faction *owner = get_faction(owner_id);
    if (owner == NULL) {
        BOOST_LOG_TRIVIAL(warning) << "No faction with id: " << owner_id;
        return NULL;
    }

    StructureType *type = structure_types[type_name];
    if (type == NULL) {
        BOOST_LOG_TRIVIAL(warning) << "No structure type: " << type_name;
        return NULL;
    }

    Tile& tile = level.tiles[position];
    if (tile.structure != NULL) {
        BOOST_LOG_TRIVIAL(warning) << "Structure already exists at: " << position;
        return NULL;
    }

    Structure *new_structure = new Structure(position, type, owner);
    tile.structure = new_structure;

    return new_structure;
}

void Game::destroy_unit_stack(int stack_id) {
    UnitStack *stack = stacks[stack_id];
    level.tiles[stack->position].stack = NULL;
    stacks.erase(stack_id);
    delete stack;
}

void Game::transfer_units(int stack_id, std::set<int> selected_units, Path path, int target_id) {
    UnitStack *stack = get_stack(stack_id);
    if (stack == NULL) {
        BOOST_LOG_TRIVIAL(warning) << "No stack with id " << stack_id;
        return;
    }
    UnitStack *target_stack = get_stack(target_id);
    if (target_stack == NULL) {
        BOOST_LOG_TRIVIAL(warning) << "No stack with id " << target_id;
        return;
    }

    MovementModel movement(&level);
    for (Path::const_iterator iter = path.begin() + 1; iter != path.end(); iter++) {
        Point pos = *iter;
        movement.move(stack, pos);
    }

    Point& new_pos = path.back();
    if (stack == target_stack) {
        level.tiles[stack->position].stack = NULL;
        stack->position = new_pos;
        level.tiles[new_pos].stack = stack;
    } else {
        stack->transfer_units(selected_units, target_stack);
    }
}

bool Game::mark_faction_ready(int faction_id, bool ready) {
    Faction *faction = get_faction(faction_id);
    if (faction == NULL) {
        BOOST_LOG_TRIVIAL(warning) << "Invalid faction: " << faction_id;
        return false;
    }
    if (faction->ready == ready) {
        return false;
    }
    faction->ready = ready;
    return true;
}

bool Game::all_factions_ready() {
    for (std::map<int, Faction *>::const_iterator iter = factions.begin(); iter != factions.end(); iter++) {
        Faction *faction = iter->second;
        if (!faction->ready)
            return false;
    }

    return true;
}

void Game::begin_turn(int turn_number) {
    for (std::map<int, UnitStack *>::iterator iter = stacks.begin(); iter != stacks.end(); iter++) {
        UnitStack *stack = iter->second;
        for (std::vector<Unit *>::iterator unit_iter = stack->units.begin(); unit_iter != stack->units.end(); unit_iter++) {
            Unit *unit = *unit_iter;
            UnitType *type = unit->type;
            unit->moves = type->moves;
        }
    }

    for (std::map<int, Faction *>::const_iterator iter = factions.begin(); iter != factions.end(); iter++) {
        Faction *faction = iter->second;
        faction->ready = false;
    }

    BOOST_LOG_TRIVIAL(debug) << "Start of turn " << turn_number;
    this->turn_number = turn_number;
    in_turn = true;
}

void Game::end_turn() {
    BOOST_LOG_TRIVIAL(debug) << "End of turn " << turn_number;
    in_turn = false;
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

Structure *Game::get_structure(const Point& position) {
    return level.tiles[position].structure;
}

int Game::get_free_stack_id() {
    return stacks.rbegin()->second->id + 1;
}
