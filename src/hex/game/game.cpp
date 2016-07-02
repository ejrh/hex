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

        TileType::pointer tile_type = tile_types.get(tile_type_name);
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

TileType::pointer Game::create_tile_type(const TileType& tile_type) {
    TileType::pointer new_tile_type = boost::make_shared<TileType>(tile_type);
    tile_types.put(tile_type.name, new_tile_type);
    return new_tile_type;
}

UnitType::pointer Game::create_unit_type(const UnitType& unit_type) {
    UnitType::pointer new_unit_type = boost::make_shared<UnitType>(unit_type);
    unit_types.put(unit_type.name, new_unit_type);
    return new_unit_type;
}

StructureType::pointer Game::create_structure_type(StructureType& structure_type) {
    StructureType::pointer new_structure_type = boost::make_shared<StructureType>(structure_type);
    structure_types.put(structure_type.name, new_structure_type);
    return new_structure_type;
}

Faction::pointer Game::create_faction(int id, const std::string& type_name, const std::string& name) {
    Faction::pointer faction = boost::make_shared<Faction>(id, type_name, name);
    factions.put(id, faction);
    return faction;
}

UnitStack::pointer Game::create_unit_stack(int id, const Point position, int owner_id) {
    if (level.tiles[position].stack) {
        throw DataError() << "Stack already exists at: " << position;
    }

    Faction::pointer owner = factions.get(owner_id);
    UnitStack::pointer new_stack = boost::make_shared<UnitStack>(id, position, owner);
    stacks.put(id, new_stack);

    level.tiles[position].stack = new_stack;

    return new_stack;
}

Unit::pointer Game::create_unit(int stack_id, const std::string& type_name) {
    UnitType::pointer type = unit_types.get(type_name);

    UnitStack::pointer stack = stacks.get(stack_id);
    Unit::pointer new_unit = boost::make_shared<Unit>();
    new_unit->type = type;
    new_unit->properties[Health] = type->properties[Health];

    stack->units.push_back(new_unit);
    return new_unit;
}

Structure::pointer Game::create_structure(const Point& position, const std::string& type_name, int owner_id) {
    Tile& tile = level.tiles[position];
    if (tile.structure) {
        throw DataError() << "Structure already exists at: " << position;
    }

    StructureType::pointer type = structure_types.get(type_name);
    Faction::pointer owner;
    if (type->has_property(Capturable))
        owner = factions.get(owner_id);
    Structure::pointer new_structure = boost::make_shared<Structure>(position, type, owner);
    tile.structure = new_structure;
    return new_structure;
}

void Game::destroy_unit_stack(int stack_id) {
    UnitStack::pointer stack = stacks.get(stack_id);
    level.tiles[stack->position].stack = UnitStack::pointer();
    stacks.remove(stack_id);
}

void Game::transfer_units(int stack_id, const IntSet selected_units, Path path, int target_id) {
    UnitStack::pointer stack = stacks.get(stack_id);
    UnitStack::pointer target_stack = stacks.get(target_id);

    MovementModel movement(&level);
    for (Path::const_iterator iter = path.begin(); iter != path.end(); iter++) {
        Point pos = *iter;
        movement.move(*stack, selected_units, pos);
    }

    Point& new_pos = path.back();
    if (stack == target_stack) {
        level.tiles[stack->position].stack = UnitStack::pointer();
        stack->position = new_pos;
        level.tiles[new_pos].stack = stack;
    } else {
        stack->transfer_units(selected_units, *target_stack);
    }
}

bool Game::mark_faction_ready(int faction_id, bool ready) {
    Faction::pointer faction = factions.get(faction_id);
    if (!faction) {
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
    for (IntMap<Faction>::const_iterator iter = factions.begin(); iter != factions.end(); iter++) {
        Faction& faction = *iter->second;
        if (!faction.ready)
            return false;
    }

    return true;
}

void Game::begin_turn(int turn_number) {
    for (IntMap<UnitStack>::iterator iter = stacks.begin(); iter != stacks.end(); iter++) {
        UnitStack::pointer stack = iter->second;
        for (std::vector<Unit::pointer>::iterator unit_iter = stack->units.begin(); unit_iter != stack->units.end(); unit_iter++) {
            Unit& unit = **unit_iter;
            UnitType& type = *unit.type;
            unit.properties[Moves] = type.properties[Moves];
            if (unit.properties[Health] > 0 && unit.properties[Health] < type.properties[Health])
                unit.properties[Health]++;
        }
    }

    for (IntMap<Faction>::const_iterator iter = factions.begin(); iter != factions.end(); iter++) {
        Faction& faction = *iter->second;
        faction.ready = false;
    }

    BOOST_LOG_TRIVIAL(debug) << "Start of turn " << turn_number;
    this->turn_number = turn_number;
    in_turn = true;
}

void Game::end_turn() {
    BOOST_LOG_TRIVIAL(debug) << "End of turn " << turn_number;
    in_turn = false;
}

int Game::get_free_stack_id() {
    return stacks.get_free_id();
}
