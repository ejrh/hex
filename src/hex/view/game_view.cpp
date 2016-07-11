#include "common.h"

#include "hex/basics/error.h"
#include "hex/basics/hexgrid.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/game/movement/movement.h"
#include "hex/game/movement/pathfinding.h"
#include "hex/messaging/message.h"
#include "hex/resources/view_def.h"
#include "hex/view/player.h"
#include "hex/view/view.h"


GameView::GameView(Game *game, Player *player, Resources *resources, MessageReceiver *dispatcher):
        game(game), player(player), level_view(&game->level), resources(resources), dispatcher(dispatcher),
        last_update(0), phase(0),
        faction_views("faction_views"), unit_stack_views("unit_stack_views"),
        selected_stack_id(0), selected_structure(), debug_mode(false) {
}

void GameView::update() {
    unsigned int ticks = SDL_GetTicks();
    if (ticks < last_update + 25)
        return;
    unsigned int update_ms = ticks - last_update;
    last_update = ticks;

    int level_bpm = 60 * 40;
    phase += frame_incr(level_bpm, update_ms);

    for (int i = 0; i < level_view.level->height; i++)
        for (int j = 0; j < level_view.level->width; j++) {
            TileViewDef::pointer view_def = level_view.tile_views[i][j].view_def;
            if (!view_def)
                continue;
            level_view.tile_views[i][j].phase += frame_incr(view_def->animation.bpm, update_ms);
        }

    for (IntMap<UnitStackView>::iterator iter = unit_stack_views.begin(); iter != unit_stack_views.end(); iter++) {
        UnitStackView& stack_view = *iter->second;
        if (stack_view.moving)
            continue;
        UnitViewDef::pointer view_def = stack_view.view_def;
        if (!view_def)
            continue;
        stack_view.update(update_ms);
    }

    level_view.ghost_visibility.clear();
    std::vector<Ghost>::iterator iter = ghosts.begin();
    while (iter != ghosts.end()) {
        Ghost& ghost = *iter;

        ghost.update(update_ms);
        if (ghost.finished)
            iter = ghosts.erase(iter);
        else
            iter++;
    }
}

void GameView::left_click_tile(const Point& tile_pos) {
    Tile& tile = level_view.level->tiles[tile_pos];
    if (tile.stack && tile.stack->id != selected_stack_id) {
        if (selected_stack_id != 0) {
            UnitStackView::pointer current_stack_view = get_stack_view(selected_stack_id);
            current_stack_view->selected = false;
        }
        UnitStack::pointer stack = tile.stack;
        UnitStackView::pointer stack_view = get_stack_view(stack->id);
        stack_view->selected = true;
        selected_stack_id = stack->id;
        selected_units.clear();
        for (unsigned int i = 0; i < stack->units.size(); i++) {
            selected_units.insert(i);
        }
        if (selected_structure) {
            StructureView::pointer structure_view = level_view.tile_views[selected_structure->position].structure_view;
            structure_view->selected = false;
        }
        selected_structure = Structure::pointer();

        if (player->has_view(stack->owner))
            set_drawn_path(stack->position, stack_view->path);
        else
            clear_drawn_path();
    } else if (tile.structure) {
        if (selected_stack_id) {
            UnitStackView::pointer stack_view = get_stack_view(selected_stack_id);
            stack_view->selected = false;
        }
        if (selected_structure) {
            StructureView::pointer structure_view = level_view.tile_views[selected_structure->position].structure_view;
            structure_view->selected = false;
        }
        selected_stack_id = 0;
        selected_structure = tile.structure;
        clear_drawn_path();
        StructureView::pointer structure_view = level_view.tile_views[selected_structure->position].structure_view;
        structure_view->selected = true;
    }
}

void GameView::right_click_tile(const Point& tile_pos) {
    if (!selected_stack_id)
        return;

    UnitStack::pointer stack = game->stacks.get(selected_stack_id);
    UnitStackView::pointer stack_view = get_stack_view(selected_stack_id);
    if (!stack || !stack_view || stack_view->locked)
        return;

    if (level_view.level->contains(tile_pos) && player->has_control(stack->owner)) {
        UnitStack::pointer selected_stack = stack->copy_subset(selected_units);
        MovementModel movement_model(level_view.level, tile_pos);
        Pathfinder pathfinder(level_view.level, &movement_model);
        pathfinder.start(*selected_stack, stack->position, tile_pos);
        pathfinder.complete();
        Path new_path;
        pathfinder.build_path(new_path);

        if (stack_view->path == new_path && !new_path.empty()) {
            int target_id = 0;
            UnitStack::pointer target_stack = level_view.level->tiles[new_path.back()].stack;
            if (target_stack) {
                if (target_stack->owner == stack->owner) {
                    if (target_stack->units.size() + stack->units.size() > MAX_UNITS)
                        return;
                }
                target_id = target_stack->id;
            }
            dispatcher->receive(create_message(UnitMove, stack->id, selected_units, new_path, target_id));
        } else {
            stack_view->path = new_path;
            set_drawn_path(stack->position, stack_view->path);
        }
    }
}

void GameView::clear_drawn_path() {
    for (std::vector<Point>::const_iterator iter = drawn_path.begin(); iter != drawn_path.end(); iter++) {
        level_view.tile_views[*iter].path_dir = -1;
    }

    drawn_path.clear();
}

void GameView::set_drawn_path(const Point& start, const Path& path) {
    // Erase existing path
    for (std::vector<Point>::const_iterator iter = drawn_path.begin(); iter != drawn_path.end(); iter++) {
        level_view.tile_views[*iter].path_dir = -1;
    }

    drawn_path = path;

    // Draw new path
    Point last = start;
    for (std::vector<Point>::const_iterator iter = drawn_path.begin(); iter != drawn_path.end(); iter++) {
        level_view.tile_views[*iter].path_dir = get_direction(last, *iter);
        last = *iter;
    }
}

void GameView::update_visibility() {
    level_view.visibility.clear();
    for (IntMap<UnitStackView>::iterator iter = unit_stack_views.begin(); iter != unit_stack_views.end(); iter++) {
        UnitStack::pointer stack = iter->second->stack;
        if (player->has_view(stack->owner) && !iter->second->moving) {
            level_view.discovered.draw(stack->position, stack->sight(), true);
            level_view.visibility.draw(stack->position, stack->sight(), true);
        }
    }

    for (int i = 0; i < level_view.level->height; i++) {
        for (int j = 0; j < level_view.level->width; j++) {
            Point tile_pos(j, i);
            TileView& tile_view = level_view.tile_views[tile_pos];
            if (!tile_view.structure_view)
                continue;
            Structure::pointer structure = tile_view.structure_view->structure;
            if (player->has_view(structure->owner)) {
                level_view.discovered.draw(tile_pos, structure->sight(), true);
                level_view.visibility.draw(tile_pos, structure->sight(), true);
            }
        }
    }
}

UnitStackView::pointer GameView::get_stack_view(int stack_id) {
    return unit_stack_views.find(stack_id);
}

TileView *GameView::get_tile_view(const Point tile_pos) {
    if (level_view.level->contains(tile_pos)) {
        return &level_view.tile_views[tile_pos];
    }

    return NULL;
}

void GameView::transfer_units(int stack_id, const IntSet selected_units, Path path, int target_id) {
    UnitStack::pointer stack = game->stacks.get(stack_id);
    UnitStack::pointer target_stack = game->stacks.get(target_id);

    if (path.empty())
        return;

    Ghost ghost(this, stack, selected_units, path, target_stack);
    ghosts.push_back(ghost);
}

void GameView::mark_ready() {
    for (IntMap<FactionView>::const_iterator iter = faction_views.begin(); iter != faction_views.end(); iter++) {
        int faction_id = iter->first;
        FactionView& faction_view = *iter->second;
        if (player->has_control(faction_view.faction)) {
            dispatcher->receive(create_message(FactionReady, faction_id, true));
        }
    }
}

void GameView::set_view_def(UnitStackView& stack_view) const {
    UnitStack::pointer& stack = stack_view.stack;
    if (!stack->units.empty()) {
        int representative = 0;
        for (int i = 1; i < stack->units.size(); i++) {
            if (stack->units[i]->has_property(Transport)) {
                representative = i;
                break;
            }
            //TODO pick most valuable
        }
        UnitType& unit_type = *stack->units[representative]->type;
        stack_view.view_def = resources->get_unit_view_def(unit_type.name);
    } else {
        stack_view.view_def.reset();
    }
}

void GameView::fix_stack_views() {
    std::vector<int> to_delete;

    for (IntMap<UnitStackView>::iterator iter = unit_stack_views.begin(); iter != unit_stack_views.end(); iter++) {
        UnitStackView& view = *iter->second;
        if (game->stacks.find(view.stack->id) != view.stack) {
            to_delete.push_back(view.stack->id);
        } else {
            set_view_def(view);
        }
    }

    for (std::vector<int>::iterator iter = to_delete.begin(); iter != to_delete.end(); iter++) {
        unit_stack_views.remove(*iter);
    }
}
