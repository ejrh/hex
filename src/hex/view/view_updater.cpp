#include "common.h"

#include "hex/basics/vector2.h"
#include "hex/messaging/serialiser.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"
#include "hex/game/game.h"
#include "hex/game/game_serialisation.h"
#include "hex/resources/resources.h"
#include "hex/resources/view_def.h"
#include "hex/game/game_messages.h"
#include "hex/view/view.h"
#include "hex/view/view_updater.h"


ViewUpdater::ViewUpdater(Game *game, GameView *game_view, Resources *resources): game(game), game_view(game_view), resources(resources) {
}

ViewUpdater::~ViewUpdater() {
}

void ViewUpdater::receive(boost::shared_ptr<Message> update) {
    apply_update(update);
}

void ViewUpdater::apply_update(boost::shared_ptr<Message> update) {
    switch (update->type) {
        case SetLevel: {
            boost::shared_ptr<SetLevelMessage> upd = boost::dynamic_pointer_cast<SetLevelMessage>(update);
            game_view->level_view.set_level(upd->level);
        } break;

        case CreateTileType:
        case CreateUnitType:
            // Ignore
            break;

        case CreateStack: {
            boost::shared_ptr<CreateStackMessage> upd = boost::dynamic_pointer_cast<CreateStackMessage>(update);
            UnitStack *stack = game->get_stack(upd->stack_id);
            if (stack == NULL) {
                return;
                //return false;
            }

            UnitViewDef *view_def = NULL;
            if (!stack->units.empty()) {
                UnitType *unit_type = stack->units[0]->type;
                view_def = resources->unit_view_defs[unit_type->name];
            }

            UnitStackView stack_view(stack, view_def);
            game_view->level_view.unit_stack_views[stack->id] = stack_view;
        } break;

        case CreateUnit: {
            boost::shared_ptr<CreateUnitMessage> upd = boost::dynamic_pointer_cast<CreateUnitMessage>(update);
            UnitStack *stack = game->get_stack(upd->stack_id);
            if (stack == NULL)
                return;
                //return false;

            if (stack->units.empty())
                return;

            UnitStackView *stack_view = &game_view->level_view.unit_stack_views[upd->stack_id];

            UnitType *unit_type = stack->units[0]->type;
            UnitViewDef *view_def = resources->unit_view_defs[unit_type->name];
            stack_view->view_def = view_def;
        } break;

        case PlayerReady: {
        } break;

        case TurnEnd: {
        } break;

        case UnitMove: {
            boost::shared_ptr<UnitMoveMessage> upd = boost::dynamic_pointer_cast<UnitMoveMessage>(update);

            UnitStack *stack = game->stacks[upd->unit];
            UnitStackView *stack_view = &game_view->level_view.unit_stack_views[stack->id];
            stack_view->path = Path();
            if (game_view->level_view.selected_stack == stack) {
                game_view->level_view.set_drawn_path(stack_view->path);
            }
            game_view->level_view.moving_unit = stack;
            game_view->level_view.moving_unit_path = upd->path;
            game_view->level_view.moving_unit_progress = 0;
        } break;

        default: {
            std::cerr << "Unhandled update of type " << update->type << " (" << get_message_type_name(update->type) << ")" << std::endl;
        }
    }
}
