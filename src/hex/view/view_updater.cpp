#include "common.h"

#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/messaging/message.h"
#include "hex/view/player.h"
#include "hex/view/tile_painter.h"
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
            game_view->level_view.resize(upd->data1, upd->data2);
        } break;

        case SetLevelData: {
            boost::shared_ptr<WrapperMessage2<Point, std::vector<std::string> > > upd = boost::dynamic_pointer_cast<WrapperMessage2<Point, std::vector<std::string> > >(update);
            Point offset = upd->data1;
            int len = upd->data2.size();
            TilePainter painter(game, game_view, resources);
            painter.repaint(offset, len);
        } break;

        case CreateFaction: {
            boost::shared_ptr<CreateFactionMessage> upd = boost::dynamic_pointer_cast<CreateFactionMessage>(update);
            Faction *faction = game->get_faction(upd->data1);
            FactionViewDef *view_def = resources->get_faction_view_def(upd->data2);
            game_view->faction_views[upd->data1] = new FactionView(faction, view_def);
        } break;

        case CreateStack: {
            boost::shared_ptr<CreateStackMessage> upd = boost::dynamic_pointer_cast<CreateStackMessage>(update);
            UnitStack *stack = game->get_stack(upd->data1);
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
            game_view->unit_stack_views[stack->id] = stack_view;
        } break;

        case CreateUnit: {
            boost::shared_ptr<CreateUnitMessage> upd = boost::dynamic_pointer_cast<CreateUnitMessage>(update);
            UnitStack *stack = game->get_stack(upd->data1);
            if (stack == NULL)
                return;
                //return false;

            if (stack->units.empty())
                return;

            UnitStackView *stack_view = &game_view->unit_stack_views[upd->data1];

            UnitType *unit_type = stack->units[0]->type;
            UnitViewDef *view_def = resources->get_unit_view_def(unit_type->name);
            stack_view->view_def = view_def;

            if (game_view->player->has_view(stack->owner)) {
                game_view->update_visibility();
            }
        } break;

        case TransferUnits: {
            boost::shared_ptr<UnitMoveMessage> upd = boost::dynamic_pointer_cast<UnitMoveMessage>(update);
            game_view->transfer_units(upd->data1, upd->data2, upd->data3, upd->data4);
        } break;

        case DestroyStack: {
            boost::shared_ptr<DestroyStackMessage> upd = boost::dynamic_pointer_cast<DestroyStackMessage>(update);
            if (game_view->selected_stack != NULL && game_view->selected_stack->id == upd->data) {
                game_view->selected_stack = NULL;
                game_view->set_drawn_path(Path());
            }
            game_view->unit_stack_views.erase(upd->data);
        } break;

        case CreateStructure: {
            boost::shared_ptr<CreateStructureMessage> upd = boost::dynamic_pointer_cast<CreateStructureMessage>(update);
            Structure *structure = game->get_structure(upd->data1);
            if (structure == NULL) {
                return;
            }
            StructureType *structure_type = structure->type;
            StructureViewDef *view_def = resources->get_structure_view_def(structure_type->name);
            StructureView *structure_view = new StructureView(structure, view_def);
            game_view->level_view.tile_views[upd->data1].structure_view = structure_view;

            if (game_view->player->has_view(structure->owner)) {
                game_view->update_visibility();
            }
        } break;

        case GrantFactionView: {
            boost::shared_ptr<GrantFactionViewMessage> upd = boost::dynamic_pointer_cast<GrantFactionViewMessage>(update);
            if (upd->data1 == game_view->player->id) {
                Faction *faction = game->get_faction(upd->data2);
                game_view->player->grant_view(faction, upd->data3);

                if (game_view->player->has_view(faction)) {
                    game_view->update_visibility();
                }
            }
        } break;

        case GrantFactionControl: {
            boost::shared_ptr<GrantFactionControlMessage> upd = boost::dynamic_pointer_cast<GrantFactionControlMessage>(update);
            if (upd->data1 == game_view->player->id) {
                Faction *faction = game->get_faction(upd->data2);
                game_view->player->grant_control(faction, upd->data3);
            }
        } break;

        case TurnBegin: {
            boost::shared_ptr<TurnBeginMessage> upd = boost::dynamic_pointer_cast<TurnBeginMessage>(update);
            std::ostringstream ss;
            ss << "Day " << upd->data;
            game_view->messages.push_back(InfoMessage(ss.str()));
        } break;

        case TurnEnd: {
        } break;

        default:
            break;
    }
}
