#include "common.h"

#include "hex/basics/hexgrid.h"
#include "hex/game/game.h"
#include "hex/game/movement/movement.h"
#include "hex/resources/view_def.h"
#include "hex/view/player.h"
#include "hex/view/view.h"


Ghost::Ghost(GameView *view, UnitStack::pointer stack, const IntSet selected_units, Path path, UnitStack::pointer target_stack):
        finished(false), view(view), step(0), progress(0) {

    // Create ghost
    UnitStack::pointer ghost_stack = stack->copy_subset(selected_units);
    stack_view = boost::make_shared<UnitStackView>(ghost_stack);
    view->set_view_def(*stack_view);
    stack_view->path = path;
    stack_view->moving = true;
    stack_view->posture = Moving;
    stack_view->selected = view->selected_stack_id == stack->id;

    // Lock target
    target_view = view->get_stack_view(target_stack->id);
    target_view->locked = true;
    if (stack == target_stack || target_stack->units.size() == 0)
        target_view->moving = true;
}

void Ghost::update(unsigned int update_ms) {
    UnitViewDef::pointer& view_def = stack_view->view_def;
    progress += frame_incr(view_def->move_speed, update_ms);
    if (progress > 1000) {
        stack_view->stack->position = stack_view->path[step];
        step++;
        progress -= 1000;
    }

    UnitStack::pointer& target = target_view->stack;
    if (step >= stack_view->path.size()) {
        view->set_view_def(*target_view);
        target_view->locked = false;
        if (target_view->moving) {
            target_view->moving = false;
            target_view->facing = stack_view->facing;
        }
        if (view->player->has_view(target->owner)) {
            view->level_view.visibility.unmask(*target);
            view->update_visibility();
        }
        if (stack_view->selected) {
            view->set_drawn_path(target->position, target_view->path);
        }
        finished = true;
        return;
    }

    Point next_pos = stack_view->path[step];
    stack_view->facing = get_direction(next_pos, stack_view->stack->position);
    stack_view->phase += frame_incr(view_def->move_animations[stack_view->facing].bpm, update_ms);
    if (view->player->has_view(target->owner)) {
        view->level_view.discovered.draw(next_pos, target->sight(), true);
        view->level_view.ghost_visibility.draw(next_pos, target->sight(), true);
    }
}
