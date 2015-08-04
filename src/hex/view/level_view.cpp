#include "common.h"

#include "hex/basics/error.h"
#include "hex/basics/hexgrid.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/game/movement.h"
#include "hex/game/pathfinding.h"
#include "hex/messaging/message.h"
#include "hex/resources/view_def.h"
#include "hex/view/view.h"


Ghost::Ghost(UnitStack *stack, Path path, int progress): stack(stack), path(path), progress(progress) {
}

LevelView::LevelView(Level *level, Resources *resources, MessageReceiver *dispatcher):
        level(level), resources(resources), dispatcher(dispatcher),
        last_update(0), selected_stack(NULL), visibility(level), discovered(level), ghost_visibility(level) {
    resize(level->width, level->height);
}

LevelView::~LevelView() {
}

void LevelView::resize(int width, int height) {
    tile_views.resize(width, height);

    visibility.resize(width, height);
    discovered.resize(width, height);
    ghost_visibility.resize(width, height);
}

// Assumes 1000 increments between frames
#define frame_incr(bpm, update_ms) ((bpm) * (update_ms) / 60)

void LevelView::update() {
    unsigned int ticks = SDL_GetTicks();
    if (ticks < last_update + 25)
        return;

    unsigned int update_ms = ticks - last_update;

    int level_bpm = 60 * 40;
    phase += frame_incr(level_bpm, update_ms);

    last_update = ticks;
    for (int i = 0; i < level->height; i++)
        for (int j = 0; j < level->width; j++) {
            TileViewDef *view_def = tile_views[i][j].view_def;
            if (view_def ==  NULL)
                continue;
            tile_views[i][j].phase += frame_incr(view_def->animation.bpm, update_ms);
        }

    for (std::map<int, UnitStackView>::iterator iter = unit_stack_views.begin(); iter != unit_stack_views.end(); iter++) {
        if (iter->second.stack == NULL || iter->second.moving)
            continue;
        UnitViewDef *view_def = iter->second.view_def;
        if (view_def == NULL)
            continue;
        iter->second.phase += frame_incr(view_def->hold_animations[iter->second.facing].bpm, update_ms);
    }

    ghost_visibility.clear();
    std::vector<Ghost>::iterator iter = ghosts.begin();
    while (iter != ghosts.end()) {
        Ghost& ghost = *iter;

        UnitStackView *stack_view = get_unit_stack_view(*ghost.stack);
        UnitViewDef *view_def = stack_view->view_def;
        ghost.progress += frame_incr(view_def->move_speed, update_ms);
        unsigned int step = ghost.progress / 1000;
        if (step >= ghost.path.size() - 1) {
            stack_view->moving = false;
            visibility.unmask(ghost.stack);
            visibility.rebuild();
            iter = ghosts.erase(iter);
        } else {
            Point &prev_pos = ghost.path[step];
            Point &next_pos = ghost.path[step + 1];
            ghost.position = next_pos;
            stack_view->facing = get_direction(next_pos, prev_pos);
            stack_view->phase += frame_incr(view_def->move_animations[stack_view->facing].bpm, update_ms);
            discovered.draw(next_pos, ghost.stack->sight(), true);
            ghost_visibility.draw(next_pos, ghost.stack->sight(), true);
            iter++;
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
        selected_units.clear();
        for (unsigned int i = 0; i < stack->units.size(); i++) {
            selected_units.insert(i);
        }

        set_drawn_path(stack_view->path);
    }
}

void LevelView::right_click_tile(const Point& tile_pos) {

    if (selected_stack == NULL)  // or dead or whatever
        return;
    UnitStackView *current_stack_view = &unit_stack_views[selected_stack->id];
    if (current_stack_view->moving)
        return;

    if (level->contains(tile_pos)) {
        MovementModel movement_model;
        Pathfinder pathfinder(level, &movement_model);
        pathfinder.start(selected_stack, selected_stack->position, tile_pos);
        pathfinder.complete();
        Path new_path;
        pathfinder.build_path(new_path);

        UnitStackView *stack_view = get_unit_stack_view(*selected_stack);

        if (stack_view->path == new_path) {
            int target_id = 0;
            UnitStack *target_stack = level->tiles[new_path.back()].stack;
            if (target_stack != NULL) {
                if (target_stack->owner == selected_stack->owner) {
                    if (target_stack->units.size() + selected_stack->units.size() > MAX_UNITS)
                        return;
                }
                target_id = target_stack->id;
            }
            dispatcher->receive(create_message(UnitMove, selected_stack->id, selected_units, new_path, target_id));
        } else {
            stack_view->path = new_path;
            set_drawn_path(stack_view->path);
        }
    }
}

bool LevelView::check_visibility(const Point& tile_pos) {
    return visibility.check(tile_pos) || ghost_visibility.check(tile_pos);
}

void LevelView::set_drawn_path(const Path& path) {
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
    std::map<int, UnitStackView>::iterator iter = unit_stack_views.find(stack.id);
    if (iter == unit_stack_views.end())
        return NULL;

    return &iter->second;
}

TileView *LevelView::get_tile_view(const Point tile_pos) {
    if (level->contains(tile_pos)) {
        return &tile_views[tile_pos];
    }

    return NULL;
}
