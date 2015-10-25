#include "common.h"

#include "hex/basics/hexgrid.h"
#include "hex/game/game.h"
#include "hex/game/movement/movement.h"
#include "hex/resources/view_def.h"
#include "hex/view/player.h"
#include "hex/view/view.h"


Ghost::Ghost(GameView *view, int target_id, Point position, Path path):
        finished(false), view(view), target_id(target_id), position(position), path(path), step(0), progress(0) {
}


void Ghost::update(unsigned int update_ms) {
    UnitStack::pointer target = view->game->stacks.get(target_id);
    UnitStackView::pointer target_view = view->get_stack_view(target_id);
    UnitViewDef::pointer view_def = target_view->view_def;
    progress += frame_incr(view_def->move_speed, update_ms);
    if (progress > 1000) {
        position = path[step];
        step++;
        progress -= 1000;
    }

    if (step >= path.size()) {
        target_view->locked = false;
        target_view->moving = false;
        if (view->player->has_view(target->owner)) {
            view->level_view.visibility.unmask(*target);
            view->update_visibility();
        }
        finished = true;
        return;
    }

    Point next_pos = path[step];
    target_view->facing = get_direction(next_pos, position);
    target_view->phase += frame_incr(view_def->move_animations[target_view->facing].bpm, update_ms);
    if (view->player->has_view(target->owner)) {
        view->level_view.discovered.draw(next_pos, target->sight(), true);
        view->level_view.ghost_visibility.draw(next_pos, target->sight(), true);
    }
}
