#include "common.h"

#include "hexgame/game/game.h"
#include "hexgame/game/movement/movement.h"
#include "hexgame/game/movement/pathfinding.h"


namespace hex {

Pivot::Pivot(Point position, Atom movement_type, int width, int height):
        position(position), movement_type(movement_type), costs(width, height) {
}

class PivotPathfinder: public PathfinderBase {
public:
    PivotPathfinder(Level *level, MovementModel *movement, Atom movement_type):
        PathfinderBase(level->width, level->height),
        movement(movement) {
        unit.type = boost::make_shared<UnitType>();
        unit.set_property(movement_type, 1);
    }

    int cost_between(const PathfinderQueueEntry& entry1, const PathfinderQueueEntry& entry2) {
        int cost = movement->cost_to(unit, entry2.point);
        return cost;
    }

    int heuristic(const PathfinderQueueEntry& entry) { return 1; }

public:
    Unit unit;
    Level *level;
    MovementModel *movement;
};

Pivot *calculate_pivot(Point position, Atom movement_type, Level *level, MovementModel *movement) {
    PivotPathfinder pathfinder(level, movement, Walking);
    pathfinder.start(position);
    pathfinder.complete();

    Pivot *pivot = new Pivot(position, movement_type, level->width, level->height);
    for (int i = 0; i < level->height; i++) {
        for (int j = 0; j < level->width; j++) {
            int cost = pathfinder.nodes[i][j].cost;
            pivot->costs[i][j] = cost;
        }
    }

    return pivot;
}

};
