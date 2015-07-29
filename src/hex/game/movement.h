#ifndef MOVEMENT_H
#define MOVEMENT_H

class UnitStack;
class Tile;

class MovementModel {
public:
    int cost_to(const UnitStack *unit, const Tile *tile) const;
};

#endif
