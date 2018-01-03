#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/basics/hexgrid.h"

#include "hexgame/game/end_of_turn.h"
#include "hexgame/game/game.h"
#include "hexgame/game/movement/movement.h"


namespace hex {

Game::Game():
        tile_types("tile types"), feature_types("feature types"),
        unit_types("unit types"), structure_types("structure types"),
        game_id(0), message_id(0), level(0, 0),
        factions("factions"), stacks("stacks") {
}

void Game::set_level_data(const Point& offset, const std::vector<std::string>& tile_data, const std::vector<std::string>& feature_data) {
    for (unsigned int i = 0; i < tile_data.size(); i++) {
        Point tile_pos(offset.x + i, offset.y);
        if (!level.contains(tile_pos)) {
            BOOST_LOG_TRIVIAL(warning) << "Tile coordinate " << tile_pos << " is outside the level";
            continue;
        }

        Tile& tile = level.tiles[tile_pos];

        const std::string& tile_type_name = tile_data[i];
        TileType::pointer tile_type = tile_types.get(tile_type_name);
        if (!tile_type) {
            BOOST_LOG_TRIVIAL(error) << "Unknown tile type: " << tile_type_name;
        } else {
            tile.type = tile_type;
        }

        const std::string& feature_type_name = feature_data[i];
        FeatureType::pointer feature_type = feature_types.get(feature_type_name);
        if (!feature_type) {
            BOOST_LOG_TRIVIAL(error) << "Unknown feature type: " << feature_type_name;
        } else {
            tile.feature_type = feature_type;
        }
    }
}

TileType::pointer Game::create_tile_type(const TileType& tile_type) {
    TileType::pointer new_tile_type = boost::make_shared<TileType>(tile_type);
    tile_types.put(tile_type.name, new_tile_type);
    return new_tile_type;
}

FeatureType::pointer Game::create_feature_type(const FeatureType& feature_type) {
    FeatureType::pointer new_feature_type = boost::make_shared<FeatureType>(feature_type);
    feature_types.put(feature_type.name, new_feature_type);
    return new_feature_type;
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
    Faction::pointer faction = boost::make_shared<Faction>(id, type_name, name, level.width, level.height);
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
    if (stack->units.size() >= MAX_UNITS) {
        throw DataError() << boost::format("Stack %d is full") % stack_id;
    }

    Unit::pointer new_unit = boost::make_shared<Unit>();
    new_unit->type = type;
    new_unit->properties[Health] = type->properties[Health];
    new_unit->properties[Moves] = type->properties[Moves];

    stack->units.push_back(new_unit);

    stack->owner->discovered.apply(*stack, true);

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

void Game::destroy_structure(const Point& position) {
    Tile& tile = level.tiles[position];
    if (!tile.structure) {
        throw DataError() << "No structure at: " << position;
    }

    tile.structure = nullptr;
}

void Game::destroy_unit_stack(int stack_id) {
    UnitStack::pointer stack = stacks.get(stack_id);
    level.tiles[stack->position].stack = UnitStack::pointer();
    stacks.remove(stack_id);
}

void Game::move_units(int stack_id, const IntSet selected_units, Point point) {
    UnitStack::pointer stack = stacks.get(stack_id);

    MovementModel movement(this);
    movement.move(*stack, selected_units, point);

    stack->owner->discovered.draw(point, stack->sight(), true);
}

void Game::transfer_units(int stack_id, const IntSet selected_units, const Path& path, int target_id) {
    UnitStack::pointer stack = stacks.get(stack_id);
    UnitStack::pointer target_stack = stacks.get(target_id);

    const Point& new_pos = path.back();
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
    for (auto iter = factions.begin(); iter != factions.end(); iter++) {
        Faction& faction = *iter->second;
        if (!faction.ready)
            return false;
    }

    return true;
}

void Game::end_turn() {
    EndOfTurn end_of_turn(this);
    end_of_turn.apply();

    in_turn = false;
}

void Game::begin_turn(int turn_number) {

    for (auto iter = factions.begin(); iter != factions.end(); iter++) {
        Faction& faction = *iter->second;
        faction.ready = false;
    }

    this->turn_number = turn_number;
    in_turn = true;
}

int Game::get_free_stack_id() {
    return stacks.get_free_id();
}

int Game::get_nearby_stacks(Point position, int radius, std::vector<UnitStack::pointer>& stacks) const {
    std::vector<Point> points = get_circle_points(position, radius, level.width, level.height);
    int num_found = 0;
    for (auto iter = points.begin(); iter != points.end(); iter++) {
        const Tile& tile = level.tiles[*iter];
        if (tile.stack) {
            stacks.push_back(tile.stack);
            num_found++;
        }
    }
    return num_found;
}

};
