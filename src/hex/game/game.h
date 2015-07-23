#ifndef GAME_H
#define GAME_H

#include "hex/basics/vector2.h"
#include "hex/basics/point.h"


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
    int moves;
};

typedef std::map<std::string, UnitType *> UnitTypeMap;

class Unit {
public:
    Unit() { }
    ~Unit() { }

public:
    UnitType *type;
    int health;
};

#define MAX_UNITS 12

class UnitStack {
public:
    UnitStack(int id, const Point position): id(id), position(position) { };
    ~UnitStack() { };

    int id;
    Point position;
    std::vector<Unit *> units;
};

class TileType {
public:
    TileType() { }
    TileType(std::string name, int walk_cost): name(name), walk_cost(walk_cost) { }
    ~TileType() { }

public:
    std::string name;
    int walk_cost;
};

typedef std::map<std::string, TileType *> TileTypeMap;

class Tile {
public:
    Tile(): type(NULL), stack(NULL) { }
    Tile(TileType *type): type(type), stack(NULL) { }

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
};

std::ostream& operator<<(std::ostream &strm, const Level& level);


class Game {
public:
    Game(): level(0, 0) { }
    ~Game() { }

    TileType *create_tile_type(TileType& tile_type);
    UnitType *create_unit_type(UnitType& unit_type);
    UnitStack *create_unit_stack(int id, const Point position);
    Unit *create_unit(int stack_id, const std::string& type_name);

    UnitStack *get_stack(int id);

public:
    TileTypeMap tile_types;
    UnitTypeMap unit_types;

    int turn_number;
    Level level;
    std::vector<Player *> players;
    std::map<int, UnitStack *> stacks;
};

#endif
