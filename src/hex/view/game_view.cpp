#include "common.h"

#include "hex/basics/error.h"
#include "hex/basics/hexgrid.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/game/movement.h"
#include "hex/game/pathfinding.h"
#include "hex/messaging/message.h"
#include "hex/resources/view_def.h"
#include "hex/view/player.h"
#include "hex/view/view.h"


Ghost::Ghost(UnitStack *stack, Path path, int progress): stack(stack), path(path), progress(progress) {
}


GameView::GameView(Game *game, Player *player, Resources *resources, MessageReceiver *dispatcher):
        game(game), player(player), level_view(&game->level), resources(resources), dispatcher(dispatcher),
        last_update(0), phase(0), selected_stack(NULL) {
}

// Assumes 1000 increments between frames
#define frame_incr(bpm, update_ms) ((bpm) * (update_ms) / 60)

void GameView::update() {
    unsigned int ticks = SDL_GetTicks();
    if (ticks < last_update + 25)
        return;

    unsigned int update_ms = ticks - last_update;

    int level_bpm = 60 * 40;
    phase += frame_incr(level_bpm, update_ms);

    last_update = ticks;
    for (int i = 0; i < level_view.level->height; i++)
        for (int j = 0; j < level_view.level->width; j++) {
            TileViewDef *view_def = level_view.tile_views[i][j].view_def;
            if (view_def ==  NULL)
                continue;
            level_view.tile_views[i][j].phase += frame_incr(view_def->animation.bpm, update_ms);
        }

    for (std::map<int, UnitStackView>::iterator iter = unit_stack_views.begin(); iter != unit_stack_views.end(); iter++) {
        if (iter->second.stack == NULL || iter->second.moving)
            continue;
        UnitViewDef *view_def = iter->second.view_def;
        if (view_def == NULL)
            continue;
        iter->second.phase += frame_incr(view_def->hold_animations[iter->second.facing].bpm, update_ms);
    }

    level_view.ghost_visibility.clear();
    std::vector<Ghost>::iterator iter = ghosts.begin();
    while (iter != ghosts.end()) {
        Ghost& ghost = *iter;

        UnitStackView *stack_view = get_unit_stack_view(*ghost.stack);
        UnitViewDef *view_def = stack_view->view_def;
        ghost.progress += frame_incr(view_def->move_speed, update_ms);
        unsigned int step = ghost.progress / 1000;
        if (step >= ghost.path.size() - 1) {
            stack_view->moving = false;
            if (player->has_view(ghost.stack->owner)) {
                level_view.visibility.unmask(ghost.stack);
                update_visibility();
            }
            iter = ghosts.erase(iter);
        } else {
            Point &prev_pos = ghost.path[step];
            Point &next_pos = ghost.path[step + 1];
            ghost.position = next_pos;
            stack_view->facing = get_direction(next_pos, prev_pos);
            stack_view->phase += frame_incr(view_def->move_animations[stack_view->facing].bpm, update_ms);
            if (player->has_view(ghost.stack->owner)) {
                level_view.discovered.draw(next_pos, ghost.stack->sight(), true);
                level_view.ghost_visibility.draw(next_pos, ghost.stack->sight(), true);
            }
            iter++;
        }
    }
}

void GameView::left_click_tile(const Point& tile_pos) {
    if (level_view.level->contains(tile_pos) && level_view.level->tiles[tile_pos].stack != NULL) {
        if (selected_stack != NULL) {
            //TileView *current_view = &tile_views[selected_stack->tile_y][selected_stack->tile_x];
            UnitStackView *current_stack_view = &unit_stack_views[selected_stack->id];
            current_stack_view->selected = false;
        }
        UnitStack *stack = level_view.level->tiles[tile_pos].stack;
        UnitStackView *stack_view = &unit_stack_views[stack->id];
        stack_view->selected = true;
        selected_stack = stack;
        selected_units.clear();
        for (unsigned int i = 0; i < stack->units.size(); i++) {
            selected_units.insert(i);
        }

        if (player->has_view(stack->owner))
            set_drawn_path(stack_view->path);
        else
            set_drawn_path(Path());
    }
}

void GameView::right_click_tile(const Point& tile_pos) {

    if (selected_stack == NULL)  // or dead or whatever
        return;
    UnitStackView *current_stack_view = &unit_stack_views[selected_stack->id];
    if (current_stack_view->moving)
        return;

    if (level_view.level->contains(tile_pos) && player->has_control(selected_stack->owner)) {
        MovementModel movement_model;
        Pathfinder pathfinder(level_view.level, &movement_model);
        pathfinder.start(selected_stack, selected_stack->position, tile_pos);
        pathfinder.complete();
        Path new_path;
        pathfinder.build_path(new_path);

        UnitStackView *stack_view = get_unit_stack_view(*selected_stack);

        if (stack_view->path == new_path) {
            int target_id = 0;
            UnitStack *target_stack = level_view.level->tiles[new_path.back()].stack;
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

void GameView::set_drawn_path(const Path& path) {
    // Erase existing path
    for (std::vector<Point>::const_iterator iter = drawn_path.begin(); iter != drawn_path.end(); iter++) {
        level_view.tile_views[*iter].path_dir = -1;
    }

    drawn_path = path;

    // Draw new path
    Point last(-1,-1);
    for (std::vector<Point>::const_iterator iter = drawn_path.begin(); iter != drawn_path.end(); iter++) {
        if (last.x != -1)
            level_view.tile_views[*iter].path_dir = get_direction(last, *iter);
        last = *iter;
    }
}

void GameView::update_visibility() {
    level_view.visibility.clear();
    for (std::map<int, UnitStackView>::iterator iter = unit_stack_views.begin(); iter != unit_stack_views.end(); iter++) {
        UnitStack *stack = iter->second.stack;
        if (player->has_view(stack->owner) && !iter->second.moving) {
            level_view.discovered.draw(stack->position, stack->sight(), true);
            level_view.visibility.draw(stack->position, stack->sight(), true);
        }
    }
}

UnitStackView *GameView::get_unit_stack_view(const UnitStack &stack) {
    std::map<int, UnitStackView>::iterator iter = unit_stack_views.find(stack.id);
    if (iter == unit_stack_views.end())
        return NULL;

    return &iter->second;
}

TileView *GameView::get_tile_view(const Point tile_pos) {
    if (level_view.level->contains(tile_pos)) {
        return &level_view.tile_views[tile_pos];
    }

    return NULL;
}
