#include "common.h"

#include "hex/basics/hexgrid.h"
#include "hex/game/game.h"
#include "hex/game/movement/movement.h"
#include "hex/game/movement/pathfinding.h"


Pathfinder::Pathfinder(Level *level, MovementModel *movement): level(level), movement(movement) {
    nodes.resize(level->width, level->height);

    clear();
}

Pathfinder::~Pathfinder() {
}

void Pathfinder::clear() {
    for (int i = 0; i < level->height; i++)
        for (int j = 0; j < level->width; j++) {
            nodes[i][j].cost = INT_MAX;
            nodes[i][j].heuristic = INT_MIN;
            nodes[i][j].state = 0;
        }

    queue.clear();
    state = CLEAR;
}

void Pathfinder::start(const UnitStack *party, const Point start_point, const Point target_point) {
    this->party = party;

    source.point = start_point.constrain(0, 0, nodes.width - 1, nodes.height - 1);
    source.node = &nodes[source.point];
    source.node->cost = 0;
    source.node->predecessor = source.point;

    target.point = target_point.constrain(0, 0, nodes.width - 1, nodes.height - 1);
    target.node = &nodes[target.point];

    source.node->state = 1;
    source.node->heuristic = heuristic(source, target);
    source.score = source.node->cost + source.node->heuristic;
    queue.push(source);
    path_head = source;
    state = RUNNING;
}

void Pathfinder::get_neighbours(const PathfinderQueueEntry& entry, PathfinderQueueEntry neighbours[]) {
    Point neighbour_points[6];
    ::get_neighbours(entry.point, neighbour_points);
    for (int i = 0; i < 6; i++) {
        if (nodes.contains(neighbour_points[i])) {
            neighbours[i].point = neighbour_points[i];
            neighbours[i].node = &nodes[neighbour_points[i]];
        } else {
            neighbours[i].node = 0;
        }
    }
}

int Pathfinder::cost_between(const PathfinderQueueEntry& entry1, const PathfinderQueueEntry& entry2) {
    int cost = movement->cost_to(party, entry2.point);
    return cost;
}

int Pathfinder::heuristic(const PathfinderQueueEntry& entry1, const PathfinderQueueEntry& entry2) {
    return distance_between(entry1.point, entry2.point);
}

void Pathfinder::step() {
    if (queue.empty()) {
        state = FINISHED;
        return;
    }

    PathfinderQueueEntry next_node = queue.top();
    queue.pop();

    if (next_node.node->heuristic < path_head.node->heuristic) {
        path_head = next_node;
    }

    if (next_node.point == target.point) {
        state = FINISHED;
        return;
    }

    next_node.node->state = 2;
    PathfinderQueueEntry neighbours[6];
    get_neighbours(next_node, neighbours);

    int offset = next_node.point.x ^ next_node.point.y; // randomise the order we examine neighbours in, for variety

    for (int i = 0; i < 6; i++) {
        PathfinderQueueEntry &neighbour = neighbours[(i + offset) % 6];
        if (neighbour.node == NULL)
            continue;
        if (neighbour.node->state == 2)
            continue;
        int step_cost = cost_between(next_node, neighbour);
        int new_cost = next_node.node->cost + step_cost;
        if (new_cost < neighbour.node->cost && step_cost < INT_MAX) {
            neighbour.node->cost = new_cost;
            neighbour.node->predecessor = next_node.point;
            if (neighbour.node->state == 0) {
                neighbour.node->heuristic = heuristic(neighbour, target);
            }
            neighbour.node->state = 1;
            neighbour.score = neighbour.node->cost + neighbour.node->heuristic;
            queue.push(neighbour);
        }
    }
}

void Pathfinder::complete() {
    while (state == RUNNING) {
        step();
    }
}

void Pathfinder::build_path(Path& path) {
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
}
