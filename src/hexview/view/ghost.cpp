#include "common.h"

#include "hexutil/basics/hexgrid.h"

#include "hexgame/game/game.h"
#include "hexgame/game/movement/movement.h"

#include "hexview/view/player.h"
#include "hexview/view/unit_painter.h"
#include "hexview/view/view.h"
#include "hexview/view/view_def.h"


namespace hex {

Ghost::Ghost(GameView *view, UnitPainter *unit_painter, UnitStack::pointer& stack, const IntSet selected_units):
        finished(false), view(view), unit_painter(unit_painter), progress(0) {

    position = stack->position;
    target_position = stack->position;
    UnitStack::pointer ghost_stack = stack->copy_subset(selected_units);
    ghost_stack->id = stack->id;
    stack_view = boost::make_shared<UnitStackView>(ghost_stack);
    view->set_view_def(*stack_view);
    stack_view->moving = true;
    stack_view->posture = Moving;
    stack_view->selected = view->selected_stack_id == stack->id;
}

void Ghost::set_target(Point target) {
    position = target_position;
    target_position = target;
    arrived_at_target = false;
    progress = 0;
    stack_view->facing = get_direction(position, target);
    stack_view->play_sound = true;
    unit_painter->repaint(*stack_view, *stack_view->stack);
}

void Ghost::retire() {
    finished = true;
}

void Ghost::update(unsigned int update_ms) {
    if (arrived_at_target)
        return;

    progress += update_ms;
    if (progress > 1000) {
        arrived_at_target = true;
        return;
    }

    stack_view->update(update_ms);
    UnitStack& stack = *stack_view->stack;
    if (view->player->has_view(stack.owner)) {
        view->level_view.ghost_visibility.draw(target_position, stack.sight(), true);
    }
}

};
