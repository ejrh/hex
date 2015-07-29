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
            boost::shared_ptr<WrapperMessage2<int, int> > upd = boost::dynamic_pointer_cast<WrapperMessage2<int, int> >(update);
            game_view->level_view.level = &game->level;
            game_view->level_view.tile_views.resize(upd->data1, upd->data2);
        } break;

        case SetLevelData: {
            boost::shared_ptr<WrapperMessage2<Point, std::vector<std::string> > > upd = boost::dynamic_pointer_cast<WrapperMessage2<Point, std::vector<std::string> > >(update);
            Point offset = upd->data1;
            std::vector<std::string>& tile_data = upd->data2;
            for (unsigned int i = 0; i < tile_data.size(); i++) {
                Point tile_pos(offset.x + i, offset.y);
                if (!game->level.contains(tile_pos)) {
                    continue;
                }

                std::string& tile_type_name = tile_data[i];
                TileType *tile_type = game->tile_types[tile_type_name];
                TileView& tile_view = game_view->level_view.tile_views[tile_pos];
                TileViewDef *view_def = game_view->level_view.resources->get_tile_view_def(tile_type->name);
                if (view_def == NULL)
                    continue;
                tile_view.view_def = view_def;
                tile_view.variation = rand();
                tile_view.phase = rand();
            }
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
                view_def = resources->get_unit_view_def(unit_type->name);
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
            UnitViewDef *view_def = resources->get_unit_view_def(unit_type->name);
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
            if (stack_view->view_def != NULL && upd->path.size() > 1) {
                game_view->level_view.moving_unit = stack;
                game_view->level_view.moving_unit_path = upd->path;
                game_view->level_view.moving_unit_progress = 0;
            }
        } break;

        default:
            break;
    }
}
