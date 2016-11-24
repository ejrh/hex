#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "hexutil/basics/point.h"

class UnitStack;
class Tile;
class UnitType;
class TileType;

class StackMovePoints {
public:
    StackMovePoints(const UnitStack& stack) {
        points.resize(stack.units.size());
        for (unsigned int i = 0; i < stack.units.size(); i++) {
            points[i] = stack.units[i]->get_property<int>(Moves);
        }
    }
    bool exhausted() const {
        for (auto iter = points.begin(); iter != points.end(); iter++) {
            if (*iter < 0)
                return true;
        }
        return false;
    }

private:
    std::vector<int> points;

    friend class MovementModel;
};

class MovementModel {
public:
    MovementModel(Level *level);
    MovementModel(Level *level, const Point& target_pos);

    bool can_enter(const UnitStack& party, const Point& tile_pos) const;
    int cost_to(const UnitStack& party, const Point& tile_pos) const;
    int cost_to(const Unit& unit, const Point& tile_pos) const;
    int admits(const UnitType& unit_type, const TileType& tile_type) const;
    void move(UnitStack& party, const IntSet& selected_units, const Point& tile_pos) const;
    int check_path(const UnitStack& stack, const Path& path) const;
    bool check_step(const UnitStack& stack, StackMovePoints *points, const Path& path, int step_num) const;

private:
    Level *level;
    Point target_pos;
};

#endif
