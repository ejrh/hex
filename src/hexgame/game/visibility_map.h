#ifndef VISIBILITY_MAP_H
#define VISIBILITY_MAP_H

#include "hexutil/basics/point.h"
#include "hexutil/basics/vector2.h"


class UnitStack;
class Level;

class VisibilityMap {
public:
    VisibilityMap(Level *level);

    void resize(int width, int height);
    void clear();
    void fill();
    void rebuild();
    void update();
    void apply(UnitStack& stack, bool visible);
    void mask(UnitStack& stack);
    void unmask(UnitStack& stack);
    void overlay(UnitStack& stack);
    void draw(const Point& point, int sight, bool visible);
    bool check(const Point& tile_pos) const;

private:
    Level *level;
    Vector2<bool> visibility;
    std::set<int> masked_stacks;
};

#endif
