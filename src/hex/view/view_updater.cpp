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
    try {
        apply_update(update);
    } catch (const DataError& err) {
        BOOST_LOG_TRIVIAL(error) << "Invalid update received; " << err.what();
    }
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
            Faction::pointer faction = game->factions.get(upd->data1);
            FactionViewDef::pointer view_def = resources->get_faction_view_def(upd->data2);
            game_view->faction_views.put(upd->data1, boost::make_shared<FactionView>(faction, view_def));
        } break;

        case CreateStack: {
            boost::shared_ptr<CreateStackMessage> upd = boost::dynamic_pointer_cast<CreateStackMessage>(update);
            UnitStack::pointer stack = game->stacks.get(upd->data1);

            UnitViewDef::pointer view_def;
            if (!stack->units.empty()) {
                UnitType& unit_type = *stack->units[0]->type;
                view_def = resources->get_unit_view_def(unit_type.name);
            }

            UnitStackView::pointer stack_view = boost::make_shared<UnitStackView>(stack, view_def);
            game_view->unit_stack_views.put(stack->id, stack_view);
        } break;

        case CreateUnit: {
            boost::shared_ptr<CreateUnitMessage> upd = boost::dynamic_pointer_cast<CreateUnitMessage>(update);
            UnitStack::pointer stack = game->stacks.get(upd->data1);

            if (stack->units.empty())
                return;

            UnitStackView::pointer stack_view = game_view->unit_stack_views.get(upd->data1);

            UnitType& unit_type = *stack->units[0]->type;
            UnitViewDef::pointer view_def = resources->get_unit_view_def(unit_type.name);
            stack_view->view_def = view_def;

            if (game_view->player->has_view(stack->owner)) {
                game_view->update_visibility();
            }
        } break;

        case DestroyStack: {
            boost::shared_ptr<DestroyStackMessage> upd = boost::dynamic_pointer_cast<DestroyStackMessage>(update);
            if (game_view->selected_stack_id == upd->data) {
                game_view->selected_stack_id = 0;
                game_view->clear_drawn_path();
            }
            game_view->unit_stack_views.remove(upd->data);
        } break;

        case CreateStructure: {
            boost::shared_ptr<CreateStructureMessage> upd = boost::dynamic_pointer_cast<CreateStructureMessage>(update);
            Structure::pointer structure = game->level.tiles[upd->data1].structure;
            if (!structure) {
                return;
            }
            StructureType::pointer structure_type = structure->type;
            StructureViewDef::pointer view_def = resources->get_structure_view_def(structure_type->name);
            StructureView::pointer structure_view = boost::make_shared<StructureView>(structure, view_def);
            game_view->level_view.tile_views[upd->data1].structure_view = structure_view;

            if (game_view->player->has_view(structure->owner)) {
                game_view->update_visibility();
            }
        } break;

        case GrantFactionView: {
            boost::shared_ptr<GrantFactionViewMessage> upd = boost::dynamic_pointer_cast<GrantFactionViewMessage>(update);
            if (upd->data1 == game_view->player->id) {
                Faction::pointer faction = game->factions.get(upd->data2);
                game_view->player->grant_view(faction, upd->data3);

                if (game_view->player->has_view(faction)) {
                    game_view->update_visibility();
                }
            }
        } break;

        case GrantFactionControl: {
            boost::shared_ptr<GrantFactionControlMessage> upd = boost::dynamic_pointer_cast<GrantFactionControlMessage>(update);
            if (upd->data1 == game_view->player->id) {
                Faction::pointer faction = game->factions.get(upd->data2);
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
