#include "common.h"

#include "hex/basics/error.h"
#include "hex/basics/hexgrid.h"
#include "hex/messaging/serialiser.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"
#include "hex/messaging/updater.h"
#include "hex/game/game.h"
#include "hex/game/game_serialisation.h"
#include "hex/game/game_messages.h"
#include "hex/resources/resources.h"
#include "hex/resources/view_def.h"

#include "hex/pathfinding.h"

#include "hex/view/view.h"

#define TILE_WIDTH 48
#define X_SPACING 32
#define Y_SPACING 32

#define SLOPE_WIDTH (TILE_WIDTH - X_SPACING)
#define SLOPE_HEIGHT (Y_SPACING/2)

LevelView::LevelView(int width, int height, Level *level, Resources *resources, MessageReceiver *dispatcher):
        width(width), height(height), level(level), resources(resources), dispatcher(dispatcher),
        last_update(0), shift_x(0), shift_y(0), x_spacing(X_SPACING), y_spacing(Y_SPACING), selected_stack(NULL), moving_unit(NULL) {
    tile_views.resize(level->width, level->height);
}

LevelView::~LevelView() {
}

// Assumes 1000 increments between frames
#define frame_incr(bpm, update_ms) ((bpm) * (update_ms) / 60)

void LevelView::update() {
    unsigned int ticks = SDL_GetTicks();
    if (ticks < last_update + 25)
        return;

    unsigned int update_ms = ticks - last_update;

    last_update = ticks;
    for (int i = 0; i < level->height; i++)
        for (int j = 0; j < level->width; j++) {
            tile_views[i][j].phase++;  //TODO
        }

    for (std::map<int, UnitStackView>::iterator iter = unit_stack_views.begin(); iter != unit_stack_views.end(); iter++) {
        if (iter->second.stack == NULL || iter->second.stack == moving_unit)
            continue;
        UnitViewDef *view_def = iter->second.view_def;
        iter->second.phase += frame_incr(view_def->hold_bpm, update_ms);
    }

    if (moving_unit != NULL) {
        UnitStackView *stack_view = get_unit_stack_view(*moving_unit);
        UnitViewDef *view_def = stack_view->view_def;
        moving_unit_progress += frame_incr(view_def->move_speed, update_ms);
        unsigned int step = moving_unit_progress / 1000;
        if (step >= moving_unit_path.size() - 1) {
            moving_unit = NULL;
        } else {
            Point &prev_pos = moving_unit_path[step];
            Point &next_pos = moving_unit_path[step + 1];
            stack_view->facing = get_direction(next_pos, prev_pos);
            stack_view->phase += frame_incr(view_def->move_bpm, update_ms);
        }
    }
}

void LevelView::set_mouse_position(int x, int y) {
    if (tile_views.contains(highlight_tile)) {
        tile_views[highlight_tile].highlighted = false;
    }

    mouse_to_tile(x, y, &highlight_tile);

    if (tile_views.contains(highlight_tile)) {
        tile_views[highlight_tile].highlighted = true;
    }
}

void LevelView::mouse_to_tile(int x, int y, Point *tile) {
    x += shift_x;
    y += shift_y;

    int x_mod = x % (2*X_SPACING);
    int x_div = x / (2*X_SPACING);

    int segment = -99;
    if (x_mod < SLOPE_WIDTH)
        segment = 0;
    else if (x_mod < X_SPACING)
        segment = 1;
    else if (x_mod < TILE_WIDTH)
        segment = 2;
    else
        segment = 3;

    if (segment == 0) {
        int y_mod = y % Y_SPACING;
        int y_div = y / Y_SPACING;
        if (y_mod*SLOPE_WIDTH < SLOPE_HEIGHT - x_mod*SLOPE_HEIGHT) {
            tile->x = 2*x_div - 1;
            tile->y = y_div - 1;
        } else if (y_mod*SLOPE_WIDTH < SLOPE_HEIGHT*SLOPE_WIDTH + x_mod*SLOPE_HEIGHT) {
            tile->x = 2*x_div;
            tile->y = y_div;
        } else {
            tile->x = 2*x_div - 1;
            tile->y = y_div;
        }
    } else if (segment == 1) {
        tile->x = 2 * x_div;
        tile->y = y / Y_SPACING;
    } else if (segment == 2) {
        x_mod -= X_SPACING;
        int y_mod = y % Y_SPACING;
        int y_div = y / Y_SPACING;
        if (y_mod*SLOPE_WIDTH < x_mod*SLOPE_HEIGHT) {
            tile->x = 2*x_div + 1;
            tile->y = y_div - 1;
        } else if (y_mod*SLOPE_WIDTH < 2*SLOPE_HEIGHT*SLOPE_WIDTH - x_mod*SLOPE_HEIGHT) {
            tile->x = 2*x_div;
            tile->y = y_div;
        } else {
            tile->x = 2*x_div + 1;
            tile->y = y_div;
        }
    } else if (segment == 3) {
        tile->x = 2 * x_div + 1;
        tile->y = (y - SLOPE_HEIGHT) / Y_SPACING;
    }
}

void LevelView::tile_to_pixel(const Point tile, int *px, int *py) {
    *px = tile.x * x_spacing - shift_x;
    *py = tile.y * y_spacing - shift_y;
    if (tile.x % 2 == 1)
        *py += y_spacing / 2;
}

void LevelView::shift(int xrel, int yrel) {
    shift_x -= xrel;
    if (shift_x < 0)
        shift_x = 0;
    if (shift_x > level->width * X_SPACING - width + SLOPE_WIDTH)
        shift_x = level->width * X_SPACING - width + SLOPE_WIDTH;

    shift_y -= yrel;
    if (shift_y < 0)
        shift_y = 0;
    if (shift_y > level->height * Y_SPACING - height + SLOPE_HEIGHT)
        shift_y = level->height * Y_SPACING - height + SLOPE_HEIGHT;
}

void LevelView::left_click(int x, int y) {
    Point tile_pos;

    mouse_to_tile(x, y, &tile_pos);

    if (level->contains(tile_pos) && level->tiles[tile_pos].stack != NULL) {
        if (selected_stack != NULL) {
            //TileView *current_view = &tile_views[selected_stack->tile_y][selected_stack->tile_x];
            UnitStackView *current_stack_view = &unit_stack_views[selected_stack->id];
            current_stack_view->selected = false;
        }
        UnitStack *stack = level->tiles[tile_pos].stack;
        UnitStackView *stack_view = &unit_stack_views[stack->id];
        stack_view->selected = true;
        selected_stack = stack;

        set_drawn_path(stack_view->path);
        trace("Selected stack %d", stack->id);
    }
}

void LevelView::right_click(int x, int y) {
    Point tile_pos;

    if (selected_stack == NULL)  // or dead or whatever
        return;

    mouse_to_tile(x, y, &tile_pos);

    if (level->contains(tile_pos)) {
        Pathfinder pathfinder(level);
        pathfinder.start(selected_stack->position, tile_pos);
        pathfinder.complete();
        Path new_path;
        pathfinder.build_path(new_path);
        UnitStackView *stack_view = get_unit_stack_view(*selected_stack);

        if (stack_view->path == new_path) {
            trace("Request move to (%d,%d)", tile_pos.x, tile_pos.y);
            dispatcher->receive(boost::make_shared<UnitMoveMessage>(selected_stack->id, new_path));
        } else {
            stack_view->path = new_path;
            set_drawn_path(stack_view->path);
            trace("Path drawn to (%d,%d)", tile_pos.x, tile_pos.y);
        }
    }
}

void LevelView::set_drawn_path(Path& path)
{
    // Erase existing path
    for (std::vector<Point>::const_iterator iter = drawn_path.begin(); iter != drawn_path.end(); iter++) {
        tile_views[*iter].path_dir = -1;
    }

    drawn_path = path;

    // Draw new path
    Point last(-1,-1);
    for (std::vector<Point>::const_iterator iter = drawn_path.begin(); iter != drawn_path.end(); iter++) {
        if (last.x != -1)
            tile_views[*iter].path_dir = get_direction(last, *iter);
        last = *iter;
    }
}

UnitStackView *LevelView::get_unit_stack_view(const UnitStack &stack) {
    return &unit_stack_views[stack.id];
}

TileView *LevelView::get_tile_view(const Point tile_pos) {
    if (level->contains(tile_pos)) {
        return &tile_views[tile_pos];
    }

    return NULL;
}
