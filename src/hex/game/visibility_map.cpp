#include "common.h"

#include "hex/basics/hexgrid.h"
#include "hex/game/game.h"
#include "hex/game/visibility_map.h"

VisibilityMap::VisibilityMap(Level *level): level(level), visibility(level->width, level->height) {
}

void VisibilityMap::clear() {
    visibility.fill(0);
}

void VisibilityMap::fill() {
    visibility.fill(-1);
}

void VisibilityMap::rebuild() {
    visibility.resize(level->tiles.width, level->tiles.height);

    visibility.fill(0);

    for (int i = 0; i < level->tiles.height; i++)
        for (int j = 0; j < level->tiles.width; j++) {
            UnitStack *stack = level->tiles[i][j].stack;
            if (stack == NULL)
                continue;
            apply(stack, true);
        }

    //for (std::vector<UnitStack *>::iterator iter = unit_stacks.begin(); iter != unit_stacks.end(); iter++) {
    //}
}

void VisibilityMap::update() {
    visibility.resize(level->tiles.width, level->tiles.height);

    for (int i = 0; i < level->tiles.height; i++)
        for (int j = 0; j < level->tiles.width; j++) {
            UnitStack *stack = level->tiles[i][j].stack;
            if (stack == NULL)
                continue;
            apply(stack, true);
        }

    //for (std::vector<UnitStack *>::iterator iter = unit_stacks.begin(); iter != unit_stacks.end(); iter++) {
    //}
}

void VisibilityMap::apply(UnitStack *stack, bool visible)
{
    int sight = stack->sight();
    int num_scanlines = 2 * sight + 1;
    int scanlines[num_scanlines];
    get_circle(stack->position, sight, scanlines);
    for (int i = 0; i < num_scanlines; i++) {
        int row = stack->position.y - sight + i;
        if (row < 0 || row >= visibility.height)
            continue;
        int x1 = stack->position.x - scanlines[i];
        int x2 = stack->position.x + scanlines[i];
        if (x1 < 0)
            x1 = 0;
        if (x2 >= visibility.width)
            x2 = visibility.width - 1;
        for (int j = x1; j <= x2; j++) {
            visibility[row][j] = visible;
        }
    }
}

void VisibilityMap::mask(UnitStack *stack)
{

}

void VisibilityMap::overlay(UnitStack *stack) {

}

bool VisibilityMap::check(const Point &tile_pos) {
    return visibility[tile_pos];
}
