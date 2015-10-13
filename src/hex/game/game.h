#ifndef GAME_H
#define GAME_H

#include "hex/basics/point.h"
#include "hex/basics/vector2.h"
#include "hex/game/traits.h"
#include "hex/game/visibility_map.h"

#define MOVE_SCALE 1

class Faction {
public:
    Faction(int id, const std::string& type_name, const std::string& name): id(id), type_name(type_name), name(name), ready(false) { }
    ~Faction() { }

public:
    int id;
    std::string type_name;
    std::string name;
    bool ready;
};

class UnitType {
public:
    UnitType() { }
    ~UnitType() { }
    bool has_ability(TraitType ability) const {
        if (abilities.count(ability) == 1)
            return true;
        return false;
    }

public:
    std::string name;
    TraitSet abilities;
    int moves;
    int sight;
};

typedef std::map<std::string, UnitType *> UnitTypeMap;

class Unit {
public:
    Unit() { }
    ~Unit() { }
    bool has_ability(TraitType ability) const {
        if (abilities.count(ability) == 1)
            return true;
        return type->has_ability(ability);
    }

public:
    UnitType *type;
    int moves;
    TraitSet abilities;
};

#define MAX_UNITS 12

class UnitStack {
public:
    UnitStack(const Point position, Faction *owner): id(0), owner(owner), position(position) { };
    UnitStack(int id, const Point position, Faction *owner): id(id), owner(owner), position(position) { };
    ~UnitStack() { };

    void transfer_units(const std::set<int>& unit_selection, UnitStack *target_stack);

    static int sight_func(int max1, const Unit *unit) {
        int max2 = unit->type->sight;
        return std::max(max1, max2);
    }
    int sight() { return std::accumulate(units.begin(), units.end(), 0, sight_func); }

public:
    int id;
    Faction *owner;
    Point position;
    std::vector<Unit *> units;
};

class StructureType {
public:
    StructureType() { }
    bool has_ability(TraitType ability) const {
        if (abilities.count(ability) == 1)
            return true;
        return false;
    }

public:
    std::string name;
    TraitSet abilities;
    int sight;
};

typedef std::map<std::string, StructureType *> StructureTypeMap;

class Structure {
public:
    Structure();
    Structure(const Point& position, StructureType *type, Faction *owner): position(position), type(type), owner(owner) { };

    int sight() { return type->has_ability(LongSight) ? 7 : 3; }

public:
    Point position;
    StructureType *type;
    Faction *owner;
};

class TileType {
public:
    TileType() { }
    ~TileType() { }

    bool has_property(TraitType trait) const {
        return properties.count(trait) == 1;
    }

public:
    std::string name;
    TraitSet properties;
};

typedef std::map<std::string, TileType *> TileTypeMap;

class Tile {
public:
    Tile(): type(NULL), stack(NULL), structure(NULL), road(false) { }
    Tile(TileType *type): type(type), stack(NULL), structure(NULL), road(false) { }

    bool has_property(TraitType trait) const {
        return type->has_property(trait);
    }

public:
    TileType *type;
    UnitStack *stack;
    Structure *structure;
    bool road;
};

class Level {
public:
    Level(int width, int height);
    ~Level();
    void resize(int width, int height);
    bool contains(const Point point) const { return tiles.contains(point); }

public:
    int width, height;
    Vector2<Tile> tiles;
};

std::ostream& operator<<(std::ostream &strm, const Level& level);


class Game {
public:
    Game(): game_id(0), message_id(0), level(0, 0) { }
    ~Game() { }

    void set_level_data(const Point& offset, const std::vector<std::string>& tile_data);
    TileType *create_tile_type(TileType& tile_type);
    UnitType *create_unit_type(UnitType& unit_type);
    StructureType *create_structure_type(StructureType& structure_type);

    Faction *create_faction(int id, const std::string& type_name, const std::string& name);
    UnitStack *create_unit_stack(int id, const Point position, int owner_id);
    Unit *create_unit(int stack_id, const std::string& type_name);
    Structure *create_structure(const Point& position, const std::string& type_name, int owner_id);
    void destroy_unit_stack(int stack_id);
    void transfer_units(int stack_id, std::set<int> selected_units, Path path, int target_id);

    bool mark_faction_ready(int faction_id, bool ready);
    bool all_factions_ready();
    void begin_turn(int turn_number);
    void end_turn();

    Faction *get_faction(int id);
    UnitStack *get_stack(int id);
    Structure *get_structure(const Point& position);
    int get_free_stack_id();

public:
    TileTypeMap tile_types;
    UnitTypeMap unit_types;
    StructureTypeMap structure_types;

    int game_id;
    int message_id;
    int turn_number;
    bool in_turn;
    Level level;
    std::map<int, Faction *> factions;
    std::map<int, UnitStack *> stacks;
};

#endif
