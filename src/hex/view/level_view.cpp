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
#include "hex/game/pathfinding.h"
#include "hex/resources/resources.h"
#include "hex/resources/view_def.h"

#include "hex/view/view.h"

LevelView::LevelView(Level *level, Resources *resources, MessageReceiver *dispatcher):
        level(level), resources(resources), dispatcher(dispatcher),
        last_update(0), selected_stack(NULL), moving_unit(NULL) {
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
        if (view_def == NULL)
            continue;
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

void LevelView::set_highlight_tile(const Point& tile_pos) {
    if (tile_views.contains(highlight_tile)) {
        tile_views[highlight_tile].highlighted = false;
    }

    highlight_tile = tile_pos;

    if (tile_views.contains(highlight_tile)) {
        tile_views[highlight_tile].highlighted = true;
    }
}

void LevelView::left_click_tile(const Point& tile_pos) {
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

void LevelView::right_click_tile(const Point& tile_pos) {

    if (selected_stack == NULL)  // or dead or whatever
        return;

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
