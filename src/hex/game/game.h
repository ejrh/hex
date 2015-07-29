#ifndef GAME_H
#define GAME_H

#include "hex/basics/vector2.h"
#include "hex/basics/point.h"
#include "hex/game/traits.h"
#include "hex/game/visibility_map.h"


class Player {
public:
    Player() { }
    ~Player() { }

public:
    std::string name;
    bool ready;
};

class UnitType {
public:
    UnitType() { }
    ~UnitType() { }

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
        if (type->abilities.count(ability) == 1)
            return true;
        return false;
    }

public:
    UnitType *type;
    int health;
    TraitSet abilities;
};

#define MAX_UNITS 12

class UnitStack {
public:
    UnitStack(int id, const Point position): id(id), position(position) { };
    ~UnitStack() { };

    static int sight_func(int max1, const Unit *unit) {
        int max2 = unit->type->sight;
        return std::max(max1, max2);
    }
    int sight() { return std::accumulate(units.begin(), units.end(), 0, sight_func); }

public:
    int id;
    Point position;
    std::vector<Unit *> units;
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
    Tile(): type(NULL), stack(NULL) { }
    Tile(TileType *type): type(type), stack(NULL) { }

    bool has_property(TraitType trait) const {
        return type->has_property(trait);
    }

public:
    TileType *type;
    UnitStack *stack;
};

class Level {
public:
    Level(int width, int height);
    ~Level();
    bool contains(const Point point) const { return tiles.contains(point); }

public:
    int width, height;
    Vector2<Tile> tiles;
    VisibilityMap visibility;
    VisibilityMap discovered;
};

std::ostream& operator<<(std::ostream &strm, const Level& level);


class Game {
public:
    Game(): game_id(0), message_id(0), level(0, 0) { }
    ~Game() { }

    TileType *create_tile_type(TileType& tile_type);
    UnitType *create_unit_type(UnitType& unit_type);
    UnitStack *create_unit_stack(int id, const Point position);
    Unit *create_unit(int stack_id, const std::string& type_name);

    UnitStack *get_stack(int id);

public:
    TileTypeMap tile_types;
    UnitTypeMap unit_types;

    int game_id;
    int message_id;
    int turn_number;
    Level level;
    std::vector<Player *> players;
    std::map<int, UnitStack *> stacks;
};

#endif
