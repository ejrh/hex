#ifndef GAME_H
#define GAME_H

#include "hex/basics/intset.h"
#include "hex/basics/objmap.h"
#include "hex/basics/point.h"
#include "hex/basics/vector2.h"
#include "hex/game/properties.h"
#include "hex/game/visibility_map.h"

#define MOVE_SCALE 1

class Faction: public boost::enable_shared_from_this<Faction> {
public:
    typedef boost::shared_ptr<Faction> pointer;

    Faction(int id, const std::string& type_name, const std::string& name): id(id), type_name(type_name), name(name), ready(false) { }
    ~Faction() { }

public:
    int id;
    std::string type_name;
    std::string name;
    bool ready;
};

class UnitType: public boost::enable_shared_from_this<UnitType> {
public:
    typedef boost::shared_ptr<UnitType> pointer;

    UnitType() { }
    ~UnitType() { }
    bool has_property(PropertyName property) const {
        return properties.contains(property);
    }
    template<typename T>
    T get_property(PropertyName property) const {
        return properties.get<T>(property);
    }

public:
    std::string name;
    Properties properties;
};

class Unit: public boost::enable_shared_from_this<Unit> {
public:
    typedef boost::shared_ptr<Unit> pointer;

    Unit() { }
    ~Unit() { }

    Unit::pointer copy() const {
        Unit::pointer unit = boost::make_shared<Unit>();
        unit->type = type;
        unit->properties = properties;
        return unit;
    }

    bool has_property(PropertyName property) const {
        return properties.contains(property, type->properties);
    }
    template<typename T>
    int get_property(PropertyName property) const {
        return properties.get<T>(property, type->properties);
    }
    template<typename T>
    void set_property(PropertyName property, const T& value) {
        properties.set<T>(property, value);
    }

public:
    UnitType::pointer type;
    Properties properties;
};

#define MAX_UNITS 12

class UnitStack: public boost::enable_shared_from_this<UnitStack> {
public:
    typedef boost::shared_ptr<UnitStack> pointer;
    typedef boost::shared_ptr<const UnitStack> const_pointer;

    UnitStack(const Point position, Faction::pointer owner): id(0), owner(owner), position(position) { };
    UnitStack(int id, const Point position, Faction::pointer owner): id(id), owner(owner), position(position) { };
    ~UnitStack() { };

    UnitStack::pointer copy_subset(const IntSet unit_selection) const;
    void transfer_units(const IntSet unit_selection, UnitStack& target_stack);

    static int sight_func(int max1, Unit::pointer unit) {
        int max2 = unit->get_property<int>(Sight);
        return std::max(max1, max2);
    }
    int sight() { return std::accumulate(units.begin(), units.end(), 0, sight_func); }

    bool has_units(const IntSet unit_selection) const;

public:
    int id;
    Faction::pointer owner;
    Point position;
    std::vector<Unit::pointer> units;
};

class StructureType: public boost::enable_shared_from_this<StructureType> {
public:
    typedef boost::shared_ptr<StructureType> pointer;

    StructureType() { }
    bool has_property(PropertyName property) const {
        return properties.contains(property);
    }
    template<typename T>
    T get_property(PropertyName property) const {
        return properties.get<T>(property);
    }

public:
    std::string name;
    Properties properties;
};

class Structure: public boost::enable_shared_from_this<Structure> {
public:
    typedef boost::shared_ptr<Structure> pointer;

    Structure();
    Structure(const Point& position, StructureType::pointer type, Faction::pointer owner): position(position), type(type), owner(owner) { };

    int sight() { return type->get_property<int>(Sight); }

public:
    Point position;
    StructureType::pointer type;
    Faction::pointer owner;
};

class TileType: public boost::enable_shared_from_this<TileType> {
public:
    typedef boost::shared_ptr<TileType> pointer;

    TileType() { }
    ~TileType() { }

    bool has_property(PropertyName property) const {
        return properties.contains(property);
    }
    template<typename T>
    T get_property(PropertyName property) const {
        return properties.get<T>(property);
    }

public:
    std::string name;
    Properties properties;
};

class Tile {
public:
    Tile(): type(), stack(), structure() { }
    Tile(TileType::pointer type): type(type), stack(), structure() { }

    bool has_property(PropertyName property) const {
        return type && type->has_property(property);
    }
    template<typename T>
    T get_property(PropertyName property) const {
        if (!type) {
            static T default_value;
            return default_value;
        }
        return type->get_property<T>(property);
    }

public:
    TileType::pointer type;
    UnitStack::pointer stack;
    Structure::pointer structure;
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
    Game(): tile_types("tile types"), unit_types("unit types"), structure_types("structure types"),
            game_id(0), message_id(0), level(0, 0),
            factions("factions"), stacks("stacks") { }
    ~Game() { }

    void set_level_data(const Point& offset, const std::vector<std::string>& tile_data);
    TileType::pointer create_tile_type(const TileType& tile_type);
    UnitType::pointer create_unit_type(const UnitType& unit_type);
    StructureType::pointer create_structure_type(StructureType& structure_type);

    Faction::pointer create_faction(int id, const std::string& type_name, const std::string& name);
    UnitStack::pointer create_unit_stack(int id, const Point position, int owner_id);
    Unit::pointer create_unit(int stack_id, const std::string& type_name);
    Structure::pointer create_structure(const Point& position, const std::string& type_name, int owner_id);
    void destroy_unit_stack(int stack_id);
    void transfer_units(int stack_id, const IntSet selected_units, Path path, int target_id);

    bool mark_faction_ready(int faction_id, bool ready);
    bool all_factions_ready();
    void begin_turn(int turn_number);
    void end_turn();

    int get_free_stack_id();
    int get_nearby_stacks(Point position, int radius, std::vector<UnitStack::pointer> stacks) const;

public:
    StrMap<TileType> tile_types;
    StrMap<UnitType> unit_types;
    StrMap<StructureType> structure_types;

    int game_id;
    int message_id;
    int turn_number;
    bool in_turn;
    Level level;
    IntMap<Faction> factions;
    IntMap<UnitStack> stacks;
};

#endif
