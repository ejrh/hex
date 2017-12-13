#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "hexutil/basics/point.h"
#include "hexutil/basics/vector2.h"


class Level;
class MovementModel;
class UnitStack;


class Pivot {
public:
    Pivot(Point position, Atom movement_type, int width, int height);

public:
    Point position;
    Atom movement_type;
    Vector2<int> costs;
};

Pivot *calculate_pivot(Point position, Atom movement_type, Level *level, MovementModel *movement);


struct PathfinderNode {
    enum State {
        None, Open, Closed
    };

    Point predecessor;
    int cost;
    int heuristic;
    State state;
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


class PathfinderBase {
public:
    PathfinderBase(int width, int height);
    virtual ~PathfinderBase();
    void add_pivot(Pivot *pivot);
    virtual void clear();
    void start(const Point start_point);
    virtual void step();
    virtual void complete();
    virtual void build_path(Path& path);

public:
    PathfinderState state;
    PathfinderQueueEntry source;
    PathfinderQueueEntry path_head;

protected:
    void get_neighbours(const PathfinderQueueEntry& entry, PathfinderQueueEntry neighbours[]);
    virtual int cost_between(const PathfinderQueueEntry& entry1, const PathfinderQueueEntry& entry2);
    virtual int heuristic(const PathfinderQueueEntry& entry);

public:
    std::vector<Pivot *> pivots;
    Vector2<PathfinderNode> nodes;
    boost::heap::priority_queue<PathfinderQueueEntry, boost::heap::compare<PathfinderNodeComparator> > queue;
    int weight;

    int nodes_pushed;
    int nodes_popped;
    long time_taken;
    int path_length;
};

class Pathfinder: public PathfinderBase {
public:
    Pathfinder(Level *level, MovementModel *movement);
    void start(const UnitStack& party, const Point start_point, const Point target_point);

    int cost_between(const PathfinderQueueEntry& entry1, const PathfinderQueueEntry& entry2);
    int heuristic(const PathfinderQueueEntry& entry);

public:
    UnitStack::const_pointer party;
    PathfinderQueueEntry target;
    Level *level;
    MovementModel *movement;
};

#endif
