#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "hex/basics/point.h"
#include "hex/basics/vector2.h"


class Level;
class MovementModel;
class UnitStack;

struct PathfinderNode {
    Point predecessor;
    int cost;
    int heuristic;
    int state;
};


struct PathfinderQueueEntry {
    Point point;
    PathfinderNode *node;
    int score;
};


enum PathfinderState {
    CLEAR, RUNNING, FINISHED
};


struct PathfinderNodeComparator {
    bool operator() (PathfinderQueueEntry const &entry1, PathfinderQueueEntry const &entry2) const {
        return entry2.score < entry1.score;
    }
};


class Pathfinder {
public:
    Pathfinder(Level *level, MovementModel *movement);
    virtual ~Pathfinder();
    virtual void clear();
    virtual void start(const UnitStack *party, const Point start_point, const Point target_point);
    virtual void step();
    virtual void complete();
    virtual void build_path(Path& path);

public:
    PathfinderState state;
    const UnitStack *party;
    PathfinderQueueEntry source;
    PathfinderQueueEntry target;
    PathfinderQueueEntry path_head;

private:
    void get_neighbours(const PathfinderQueueEntry& entry, PathfinderQueueEntry neighbours[]);
    int cost_between(const PathfinderQueueEntry& entry1, const PathfinderQueueEntry& entry2);
    int heuristic(const PathfinderQueueEntry& entry1, const PathfinderQueueEntry& entry2);

public:
    Level *level;
    MovementModel *movement;
    Vector2<PathfinderNode> nodes;
    boost::heap::priority_queue<PathfinderQueueEntry, boost::heap::compare<PathfinderNodeComparator> > queue;
};

#endif
