#include "common.h"

#include "hex/ai/ai.h"
#include "hex/ai/ai_updater.h"
#include "hex/basics/error.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/game/movement/movement.h"
#include "hex/game/movement/pathfinding.h"
#include "hex/messaging/receiver.h"
#include "hex/messaging/queue.h"


Ai::Ai(const std::string& faction_type, MessageReceiver *dispatcher):
        faction_type(faction_type), dispatcher(dispatcher),
        last_update(0), started(false) {
    updater = new AiUpdater(this);
    receiver = new MessageQueue(1000);
}

Ai::~Ai() {
    delete receiver;
    delete updater;
}

void Ai::start() {
    started = true;
    ai_thread = boost::thread(&Ai::run_thread, this);
}

void Ai::stop() {
    started = false;
    ai_thread.join();
}

MessageReceiver *Ai::get_receiver() const {
    return receiver;
}

void Ai::run_thread() {
    while (started) {
        update();
        SDL_Delay(100);
    }
}

void Ai::update() {
    // Process incoming messages
    int num_messages = receiver->flush(updater);
    if (num_messages > 0) {
        BOOST_LOG_TRIVIAL(debug) << "Ai received " << num_messages << " incoming messages";
    }

    // Perform periodic update
    unsigned int ticks = SDL_GetTicks();
    if (ticks < last_update + 10000)
        return;
    last_update = ticks;

    for (IntMap<UnitStack>::iterator iter = game.stacks.begin(); iter != game.stacks.end(); iter++) {
        UnitStack& stack = *iter->second;
        if (stack.owner == faction) {
            update_unit_stack(stack);
        }
    }

    if (!faction->ready) {
        dispatcher->receive(create_message(FactionReady, faction->id, true));
    }
}

void Ai::update_unit_stack(UnitStack& stack) {
    UnitStack::pointer enemy = get_nearest_enemy(stack);
    Point tile_pos(stack.position);
    if (enemy) {
        BOOST_LOG_TRIVIAL(info) << "Saw enemy: " << enemy->id;
        tile_pos = enemy->position;
    } else {
        tile_pos.x += (rand() % 5) - (rand() % 5);
        tile_pos.y += (rand() % 5) - (rand() % 5);
    }

    MovementModel movement_model(&game.level);
    Pathfinder pathfinder(&game.level, &movement_model);
    pathfinder.start(stack, stack.position, tile_pos);
    pathfinder.complete();
    Path new_path;
    pathfinder.build_path(new_path);

    if (new_path.size() > 10)
        new_path.resize(10);

    if (new_path.empty())
        return;

    UnitStack::pointer target_stack = game.level.tiles[new_path.back()].stack;
    if (target_stack) {
        return;
    }

    IntSet selected_units;
    for (unsigned int i = 0; i < stack.units.size(); i++) {
        selected_units.insert(i);
    }

    dispatcher->receive(create_message(UnitMove, stack.id, selected_units, new_path, 0));
}

UnitStack::pointer Ai::get_nearest_enemy(UnitStack& stack) {
    std::vector<UnitStack::pointer> stacks;
    game.get_nearby_stacks(stack.position, stack.sight(), stacks);
    for (std::vector<UnitStack::pointer>::iterator iter = stacks.begin(); iter != stacks.end(); iter++) {
        if ((*iter)->owner != stack.owner)
            return *iter;
    }
    return UnitStack::pointer();
}
