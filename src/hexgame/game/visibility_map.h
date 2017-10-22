#ifndef VISIBILITY_MAP_H
#define VISIBILITY_MAP_H

#include "hexutil/basics/point.h"
#include "hexutil/basics/vector2.h"


class UnitStack;
class Level;

class VisibilityMap {
public:
    VisibilityMap(const Level& level);
    VisibilityMap(int width, int height);

    void resize(int width, int height);
    void clear();
    void fill();
    void rebuild(const Level& level);
    void apply(const UnitStack& stack, bool visible);
    void draw(const Point& point, int sight, bool visible);

    bool check(const Point& tile_pos) const;

    void load(int row, const std::vector<std::string>& data);
    void save(int row, std::vector<std::string>& data) const;

private:
    Vector2<bool> visibility;
};

class VisibilityMapUnion {
public:
    void clear();
    void add(const VisibilityMap *map);
    void remove(const VisibilityMap *map);
    bool check(const Point& tile_pos) const;

private:
    std::vector<const VisibilityMap *> maps;
};

#endif
