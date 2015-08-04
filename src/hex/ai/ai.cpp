#include "common.h"

#include "hex/ai/ai.h"
#include "hex/basics/error.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/game/movement.h"
#include "hex/game/pathfinding.h"
#include "hex/messaging/receiver.h"


Ai::Ai(Game *game, const std::string& faction_type, MessageReceiver *dispatcher):
        game(game), faction_type(faction_type), dispatcher(dispatcher),
        last_update(0) {
}

void Ai::update() {
    unsigned int ticks = SDL_GetTicks();
    if (ticks < last_update + 10000)
        return;

    last_update = ticks;

    for (std::map<int, UnitStack*>::iterator iter = game->stacks.begin(); iter != game->stacks.end(); iter++) {
        UnitStack *stack = iter->second;
        if (stack->owner == faction) {
            update_unit_stack(stack);
        }
    }
}

void Ai::update_unit_stack(UnitStack *stack) {
    MovementModel movement_model;
    Pathfinder pathfinder(&game->level, &movement_model);
    Point tile_pos(rand() % game->level.width, rand() % game->level.height);
    pathfinder.start(stack, stack->position, tile_pos);
    pathfinder.complete();
    Path new_path;
    pathfinder.build_path(new_path);

    if (new_path.size() > 10)
        new_path.resize(10);

    UnitStack *target_stack = game->level.tiles[new_path.back()].stack;
    if (target_stack != NULL) {
        return;
    }

    std::set<int> selected_units;
    for (unsigned int i = 0; i < stack->units.size(); i++) {
        selected_units.insert(i);
    }

    dispatcher->receive(create_message(UnitMove, stack->id, selected_units, new_path, 0));
}
