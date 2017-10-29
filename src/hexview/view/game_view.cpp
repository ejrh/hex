#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/basics/hexgrid.h"
#include "hexutil/messaging/message.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"
#include "hexgame/game/movement/movement.h"
#include "hexgame/game/movement/pathfinding.h"

#include "hexview/resources/view_def.h"
#include "hexview/editor/editor.h"
#include "hexview/view/player.h"
#include "hexview/view/unit_painter.h"
#include "hexview/view/view.h"


GameView::GameView(Game *game, Player *player, Resources *resources, MessageReceiver *dispatcher):
        game(game), player(player), level_view(&game->level), resources(resources), unit_painter(game, this, resources), dispatcher(dispatcher),
        last_update(0), phase(0),
        faction_views("faction_views"), unit_stack_views("unit_stack_views"),
        selected_stack_id(0), selected_structure(), debug_mode(false),
        ghost_counter("view.ghost") {
    update_player();
}

void GameView::update() {
    unsigned int ticks = SDL_GetTicks();
    if (ticks < last_update + 25)
        return;
    unsigned int update_ms = ticks - last_update;
    last_update = ticks;

    phase += update_ms;

    for (int i = 0; i < level_view.level->height; i++)
        for (int j = 0; j < level_view.level->width; j++) {
            TileViewDef::pointer view_def = level_view.tile_views[i][j].view_def;
            if (!view_def)
                continue;
        }

    for (auto iter = unit_stack_views.begin(); iter != unit_stack_views.end(); iter++) {
        UnitStackView& stack_view = *iter->second;
        if (stack_view.moving)
            continue;
        UnitViewDef::pointer view_def = stack_view.view_def;
        if (!view_def)
            continue;
        stack_view.update(update_ms);
    }

    level_view.ghost_visibility.clear();
    auto iter = ghosts.begin();
    while (iter != ghosts.end()) {
        Ghost& ghost = *iter;

        ghost.update(update_ms);
        if (ghost.finished)
            iter = ghosts.erase(iter);
        else
            iter++;
    }
}

void GameView::update_player() {
    level_view.discovered.clear();

    for (auto iter = game->factions.begin(); iter != game->factions.end(); iter++) {
        if (player->has_view(iter->second)) {
            level_view.discovered.add(&iter->second->discovered);
        }
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
        Path new_path = find_path(*selected_stack, selected_stack->position, tile_pos);

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

Path GameView::find_path(UnitStack& party, const Point& from_pos, const Point& to_pos) {
    MovementModel movement_model(level_view.level, to_pos);
    Pathfinder pathfinder(level_view.level, &movement_model);
    pathfinder.start(party, from_pos, to_pos);
    pathfinder.complete();
    Path new_path;
    pathfinder.build_path(new_path);
    return new_path;
}

void GameView::clear_drawn_path() {
    for (auto iter = drawn_path.begin(); iter != drawn_path.end(); iter++) {
        level_view.tile_views[*iter].path_dir = -1;
    }

    drawn_path.clear();
}

void GameView::set_drawn_path(const Point& start, const Path& path) {
    // Erase existing path
    for (auto iter = drawn_path.begin(); iter != drawn_path.end(); iter++) {
        level_view.tile_views[*iter].path_dir = -1;
    }

    drawn_path = path;
    if (drawn_path.size() == 0)
        return;

    // Draw new path
    Point last = start;
    for (auto iter = drawn_path.begin(); iter != drawn_path.end(); iter++) {
        int arrow_num = get_direction(last, *iter);
        level_view.tile_views[*iter].path_dir = arrow_num;
        last = *iter;
    }
    level_view.tile_views[last].path_dir |= TileView::PATH_END;
}

void GameView::update_visibility() {
    level_view.visibility.clear();
    for (auto iter = unit_stack_views.begin(); iter != unit_stack_views.end(); iter++) {
        UnitStack::pointer stack = iter->second->stack;
        if (player->has_view(stack->owner) && !iter->second->moving) {
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
            if (structure->owner && player->has_view(structure->owner)) {
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

    UnitStackView::pointer stack_view = unit_stack_views.get(stack_id);
    if (selected_units.size() == stack->units.size()) {
        Point from_pos = *path.rbegin();
        Point tile_pos = stack_view->path.empty() ? stack->position : *stack_view->path.rbegin();
        stack_view->path = find_path(*stack, from_pos, tile_pos);
    } else {
        UnitStackView::pointer target_stack_view = unit_stack_views.get(target_id);
        Point from_pos = *path.rbegin();
        Point tile_pos = stack_view->path.empty() ? stack->position : *stack_view->path.rbegin();
        target_stack_view->path = find_path(*stack, from_pos, tile_pos);
    }

    if (selected_stack_id == stack_id) {
        clear_drawn_path();
    }

    Ghost ghost(this, &unit_painter, stack, selected_units, path, target_stack);
    ghosts.push_back(ghost);

    ++ghost_counter;
}

void GameView::mark_ready() {
    for (auto iter = faction_views.begin(); iter != faction_views.end(); iter++) {
        int faction_id = iter->first;
        FactionView& faction_view = *iter->second;
        if (player->has_control(faction_view.faction)) {
            dispatcher->receive(create_message(FactionReady, faction_id, true));
        }
    }
}

void GameView::set_view_def(UnitStackView& stack_view) const {
    stack_view.set_representative(resources);
}

void GameView::fix_stack_views() {
    std::vector<int> to_delete;

    for (auto iter = unit_stack_views.begin(); iter != unit_stack_views.end(); iter++) {
        UnitStackView& view = *iter->second;
        if (game->stacks.find(view.stack->id) != view.stack) {
            to_delete.push_back(view.stack->id);
        } else {
            set_view_def(view);
        }
    }

    for (auto iter = to_delete.begin(); iter != to_delete.end(); iter++) {
        unit_stack_views.remove(*iter);
    }
}
