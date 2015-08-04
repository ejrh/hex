#ifndef MOVEMENT_H
#define MOVEMENT_H

class UnitStack;
class Tile;
class UnitType;
class TileType;

class MovementModel {
public:
    int cost_to(const UnitStack *unit, const Tile *tile) const;
    int admits(const UnitType *unit_type, const TileType *tile_type) const;
};

#endif
