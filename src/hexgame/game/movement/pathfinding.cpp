#include "common.h"

#include "hexutil/basics/hexgrid.h"
#include "hexutil/basics/statistics.h"

#include "hexgame/game/game.h"
#include "hexgame/game/movement/movement.h"
#include "hexgame/game/movement/pathfinding.h"


namespace hex {

PathfinderBase::PathfinderBase(int width, int height) {
    nodes.resize(width, height);

    clear();
}

PathfinderBase::~PathfinderBase() {
}

void PathfinderBase::add_pivot(Pivot *pivot) {
    pivots.push_back(pivot);
}

void PathfinderBase::clear() {
    state = CLEAR;

    pivots.clear();

    for (int i = 0; i < nodes.height; i++)
        for (int j = 0; j < nodes.width; j++) {
            nodes[i][j].cost = INT_MAX;
            nodes[i][j].heuristic = INT_MIN;
            nodes[i][j].state = PathfinderNode::State::None;
        }

    queue.clear();
    weight = 1;

    nodes_pushed = 0;
    nodes_popped = 0;
    time_taken = 0;
    path_length = 0;
}

void PathfinderBase::start(const Point start_point) {
    source.point = start_point.constrain(0, 0, nodes.width - 1, nodes.height - 1);
    source.node = &nodes[source.point];
    source.node->cost = 0;
    source.node->predecessor = source.point;

    source.node->state = PathfinderNode::State::Open;
    source.node->heuristic = heuristic(source);
    source.score = source.node->cost + source.node->heuristic * weight;
    queue.push(source);
    path_head = source;
    state = RUNNING;
}

void PathfinderBase::get_neighbours(const PathfinderQueueEntry& entry, PathfinderQueueEntry neighbours[]) {
    PointNeighbours neighbour_points = hex::get_neighbours(entry.point);
    for (int i = 0; i < 6; i++) {
        if (nodes.contains(neighbour_points[i])) {
            neighbours[i].point = neighbour_points[i];
            neighbours[i].node = &nodes[neighbour_points[i]];
        } else {
            neighbours[i].node = 0;
        }
    }
}

int PathfinderBase::cost_between(const PathfinderQueueEntry& entry1, const PathfinderQueueEntry& entry2) {
    return 1;
}

int PathfinderBase::heuristic(const PathfinderQueueEntry& entry) {
    return 1;
}

void PathfinderBase::step() {
    long t0 = std::clock() * 1000000 / CLOCKS_PER_SEC;
    if (queue.empty()) {
        state = FINISHED;
        time_taken += std::clock() * 1000000 / CLOCKS_PER_SEC - t0;
        return;
    }

    PathfinderQueueEntry next_node = queue.top();
    queue.pop();

    nodes_popped++;

    if (next_node.node->heuristic < path_head.node->heuristic) {
        path_head = next_node;
    }

    if (next_node.node->heuristic == 0) {
        state = FINISHED;
        time_taken += std::clock() * 1000000 / CLOCKS_PER_SEC - t0;
        return;
    }

    next_node.node->state = PathfinderNode::State::Closed;
    PathfinderQueueEntry neighbours[6];
    get_neighbours(next_node, neighbours);

    int offset = next_node.point.x ^ next_node.point.y; // randomise the order we examine neighbours in, for variety

    int incoming_dir = get_direction(next_node.point, next_node.node->predecessor);

    for (int i = 0; i < 6; i++) {
        int dir = (i + offset) % 6;
        if (next_node.node->cost != 0) {
            int relative_dir = (incoming_dir - dir + 6) % 6;
            if (relative_dir == 0 || relative_dir == 1 || relative_dir == 5) {
                continue;
            }
        }

        PathfinderQueueEntry &neighbour = neighbours[dir];
        if (neighbour.node == NULL)
            continue;
        if (neighbour.node->state == PathfinderNode::State::Closed)
            continue;
        int step_cost = cost_between(next_node, neighbour);
        int new_cost = next_node.node->cost + step_cost;
        if (new_cost < neighbour.node->cost && step_cost < INT_MAX) {
            neighbour.node->cost = new_cost;
            neighbour.node->predecessor = next_node.point;
            if (neighbour.node->state == PathfinderNode::State::None) {
                neighbour.node->heuristic = heuristic(neighbour);
            }
            neighbour.node->state = PathfinderNode::State::Open;
            neighbour.score = neighbour.node->cost + neighbour.node->heuristic * weight;
            queue.push(neighbour);
            nodes_pushed++;
        }
    }

    time_taken += std::clock() * 1000000 / CLOCKS_PER_SEC - t0;
}

void PathfinderBase::complete() {
    while (state == RUNNING) {
        step();
    }
}

void PathfinderBase::build_path(Path& path) {
    path.clear();

    PathfinderNode *node = path_head.node;
    Point point = path_head.point;
    while (true) {
        Point pred = node->predecessor;

        if (point == pred)
            break;

        path.push_back(point);

        node = &nodes[pred];
        point = pred;
    }

    std::reverse(path.begin(), path.end());
    path_length = path.size();
}


Pathfinder::Pathfinder(Level *level, MovementModel *movement):
        PathfinderBase(level->width, level->height),
        level(level), movement(movement) {
    nodes.resize(level->width, level->height);

    clear();
}

void Pathfinder::start(const UnitStack& party, const Point start_point, const Point target_point) {
    this->party = party.shared_from_this();

    target.point = target_point.constrain(0, 0, nodes.width - 1, nodes.height - 1);
    target.node = &nodes[target.point];

    PathfinderBase::start(start_point);
}

int Pathfinder::cost_between(const PathfinderQueueEntry& entry1, const PathfinderQueueEntry& entry2) {
    int cost = movement->cost_to(*party, entry2.point);
    return cost;
}

int Pathfinder::heuristic(const PathfinderQueueEntry& entry) {
    if (entry.point == target.point)
        return 0;
    int cost = movement->cost_to(*party, entry.point);
    if (cost > 32)
        cost = 32;
    int h = distance_between(entry.point, target.point) * cost;

    for (auto iter = pivots.begin(); iter != pivots.end(); iter++) {
        int l1 = (*iter)->costs[entry.point];
        int l2 = (*iter)->costs[target.point];
        if (l1 == INT_MAX || l2 == INT_MAX)
            continue;
        int lh = abs(l1 - l2);
        if (lh > h)
            h = lh;
    }
    return h;

}

};
