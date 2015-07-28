#ifndef VISIBILITY_MAP_H
#define VISIBILITY_MAP_H

#include "hex/basics/point.h"
#include "hex/basics/vector2.h"

class UnitStack;
class Level;

class VisibilityMap {
public:
    VisibilityMap(Level *level);

    void clear();
    void fill();
    void rebuild();
    void update();
    void apply(UnitStack *stack, bool visible);
    void mask(UnitStack *stack);
    void overlay(UnitStack *stack);
    bool check(const Point &tile_pos);

private:
    Level *level;
    Vector2<unsigned int> visibility;
};

#endif
