#include "common.h"

#include "hexutil/basics/hexgrid.h"

#include "hex/game/game.h"
#include "hex/game/visibility_map.h"


VisibilityMap::VisibilityMap(Level *level) {
    this->level = level;
    resize(level->tiles.width, level->tiles.height);
}

void VisibilityMap::resize(int width, int height) {
    visibility.resize(width, height);
}

void VisibilityMap::clear() {
    visibility.resize(level->tiles.width, level->tiles.height);
    visibility.fill(false);
}

void VisibilityMap::fill() {
    visibility.resize(level->tiles.width, level->tiles.height);
    visibility.fill(true);
}

void VisibilityMap::rebuild() {
    visibility.resize(level->tiles.width, level->tiles.height);

    visibility.fill(false);

    for (int i = 0; i < level->tiles.height; i++)
        for (int j = 0; j < level->tiles.width; j++) {
            UnitStack::pointer stack = level->tiles[i][j].stack;
            if (!stack || masked_stacks.find(stack->id) != masked_stacks.end())
                continue;
            apply(*stack, true);
        }

    //for (auto iter = unit_stacks.begin(); iter != unit_stacks.end(); iter++) {
    //}
}

void VisibilityMap::update() {
    visibility.resize(level->tiles.width, level->tiles.height);

    for (int i = 0; i < level->tiles.height; i++)
        for (int j = 0; j < level->tiles.width; j++) {
            UnitStack::pointer stack = level->tiles[i][j].stack;
            if (!stack || masked_stacks.find(stack->id) != masked_stacks.end())
                continue;
            apply(*stack, true);
        }

    //for (auto iter = unit_stacks.begin(); iter != unit_stacks.end(); iter++) {
    //}
}

void VisibilityMap::apply(UnitStack& stack, bool visible)
{
    draw(stack.position, stack.sight(), visible);
}

void VisibilityMap::mask(UnitStack& stack) {
    masked_stacks.insert(stack.id);
}

void VisibilityMap::unmask(UnitStack& stack) {
    masked_stacks.erase(stack.id);
}

void VisibilityMap::overlay(UnitStack& stack) {
}

void VisibilityMap::draw(const Point& point, int sight, bool visible)
{
    int num_scanlines = 2 * sight + 1;
    std::vector<int> scanlines(num_scanlines);
    get_circle(point, sight, scanlines);
    for (int i = 0; i < num_scanlines; i++) {
        int row = point.y - sight + i;
        if (row < 0 || row >= visibility.height)
            continue;
        int x1 = point.x - scanlines[i];
        int x2 = point.x + scanlines[i];
        if (x1 < 0)
            x1 = 0;
        if (x2 >= visibility.width)
            x2 = visibility.width - 1;
        for (int j = x1; j <= x2; j++) {
            visibility[row][j] = visible;
        }
    }
}

bool VisibilityMap::check(const Point &tile_pos) const {
    if (!visibility.contains(tile_pos))
        std::cout << "!";
    return visibility[tile_pos];
}
