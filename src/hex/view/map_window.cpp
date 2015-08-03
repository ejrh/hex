#include "common.h"

#include "hex/graphics/graphics.h"
#include "hex/game/game.h"
#include "hex/view/view.h"
#include "hex/view/level_renderer.h"
#include "hex/view/level_window.h"
#include "hex/view/map_window.h"

MapWindow::MapWindow(int x, int y, int width, int height, GameView *view, LevelWindow *level_window, Graphics *graphics, Resources *resources):
        UiWindow(x, y, width, height), view(view), level_window(level_window), graphics(graphics), resources(resources) {
}

MapWindow::~MapWindow() {
}

void MapWindow::mouse_to_tile(int x, int y, Point *tile) {
}

void MapWindow::tile_to_pixel(const Point tile, int *px, int *py) {
    *px = x + 4 + tile.x*4;
    *py = y + 4 + tile.y*4;
    if (tile.x % 2 == 1)
        py -= 2;
}

void MapWindow::left_click(int x, int y) {
}

void MapWindow::right_click(int x, int y) {
}


void MapWindow::draw() {
    graphics->fill_rectangle(75,75,50, x, y, width, height);

    for (int i = 0; i < view->level_view.tile_views.height; i++)
        for (int j = 0; j < view->level_view.tile_views.width; j++) {
            if (!view->level_view.discovered.check(Point(j, i)))
                continue;

            TileView& tile_view = view->level_view.tile_views[i][j];

            int r = tile_view.view_def->r;
            int g = tile_view.view_def->g;
            int b = tile_view.view_def->b;
            if (!view->level_view.check_visibility(Point(j, i))) {
                r = (r + 75) / 2;
                g = (g + 75) / 2;
                b = (b + 50) / 2;
            }

            int px, py;
            tile_to_pixel(Point(j, i), &px, &py);

            graphics->fill_rectangle(r,g,b, px, py, 4, 4);
        }

    for (std::map<int, UnitStackView>::iterator iter = view->level_view.unit_stack_views.begin(); iter != view->level_view.unit_stack_views.end(); iter++) {
        UnitStack *stack = iter->second.stack;
        if (iter->second.moving || !view->level_view.check_visibility(stack->position))
            continue;

        int px, py;
        tile_to_pixel(stack->position, &px, &py);

        Faction *owner = stack->owner;
        FactionView *faction_view = view->faction_views[owner->id];
        FactionViewDef *faction_view_def = faction_view->view_def;

        graphics->fill_rectangle(faction_view_def->r, faction_view_def->g, faction_view_def->b, px, py, 4, 4);
    }

    for (std::vector<Ghost>::iterator iter = view->level_view.ghosts.begin(); iter != view->level_view.ghosts.end(); iter++) {
        Ghost& ghost = *iter;
        UnitStack *stack = ghost.stack;
        if (!view->level_view.check_visibility(ghost.position))
            continue;

        int px, py;
        tile_to_pixel(ghost.position, &px, &py);

        Faction *owner = stack->owner;
        FactionView *faction_view = view->faction_views[owner->id];
        FactionViewDef *faction_view_def = faction_view->view_def;

        graphics->fill_rectangle(faction_view_def->r, faction_view_def->g, faction_view_def->b, px, py, 4, 4);
    }

    Point point1, point2;
    int px1, py1, px2, py2;
    level_window->mouse_to_tile(0, 0, &point1);
    tile_to_pixel(point1, &px1, &py1);
    level_window->mouse_to_tile(level_window->width, level_window->height, &point2);
    tile_to_pixel(point2, &px2, &py2);
    graphics->draw_rectangle(255,255,255, px1, py1, px2 - px1, py2 - py1);
}
