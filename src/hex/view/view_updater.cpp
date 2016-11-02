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
        case ClearGame: {
            game_view->level_view.tile_views.resize(0, 0);
            game_view->faction_views.clear();
            game_view->unit_stack_views.clear();
            game_view->ghosts.clear();
        } break;

        case SetLevel: {
            auto upd = boost::dynamic_pointer_cast<SetLevelMessage>(update);
            game_view->level_view.level = &game->level;
            game_view->level_view.resize(upd->data1, upd->data2);
        } break;

        case SetLevelData: {
            auto upd = boost::dynamic_pointer_cast<SetLevelDataMessage>(update);
            Point offset = upd->data1;
            int len = upd->data2.size();
            TilePainter painter(game, game_view, resources);
            painter.repaint(offset, len);
        } break;

        case CreateFaction: {
            auto upd = boost::dynamic_pointer_cast<CreateFactionMessage>(update);
            Faction::pointer faction = game->factions.get(upd->data1);
            FactionViewDef::pointer view_def = resources->get_faction_view_def(upd->data2);
            game_view->faction_views.put(upd->data1, boost::make_shared<FactionView>(faction, view_def));
        } break;

        case CreateStack: {
            auto upd = boost::dynamic_pointer_cast<CreateStackMessage>(update);
            UnitStack::pointer stack = game->stacks.get(upd->data1);
            UnitStackView::pointer stack_view = boost::make_shared<UnitStackView>(stack);
            game_view->set_view_def(*stack_view);
            game_view->unit_stack_views.put(stack->id, stack_view);
        } break;

        case CreateUnit: {
            auto upd = boost::dynamic_pointer_cast<CreateUnitMessage>(update);
            UnitStack::pointer stack = game->stacks.get(upd->data1);

            if (stack->units.empty())
                return;

            UnitStackView::pointer stack_view = game_view->unit_stack_views.get(upd->data1);
            game_view->set_view_def(*stack_view);

            if (game_view->player->has_view(stack->owner)) {
                game_view->update_visibility();
            }
        } break;

        case TransferUnits: {
            auto upd = boost::dynamic_pointer_cast<UnitMoveMessage>(update);
            int old_stack_id = upd->data1;
            UnitStackView::pointer old_stack = game_view->unit_stack_views.find(old_stack_id);
            if (old_stack) {
                game_view->set_view_def(*old_stack);
            }
        } break;

        case DestroyStack: {
            auto upd = boost::dynamic_pointer_cast<DestroyStackMessage>(update);
            if (game_view->selected_stack_id == upd->data) {
                game_view->selected_stack_id = 0;
                game_view->clear_drawn_path();
            }
            game_view->unit_stack_views.remove(upd->data);
        } break;

        case CreateStructure: {
            auto upd = boost::dynamic_pointer_cast<CreateStructureMessage>(update);
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
            auto upd = boost::dynamic_pointer_cast<GrantFactionViewMessage>(update);
            if (upd->data1 == game_view->player->id) {
                Faction::pointer faction = game->factions.get(upd->data2);
                game_view->player->grant_view(faction, upd->data3);

                if (game_view->player->has_view(faction)) {
                    game_view->update_visibility();
                }
            }
        } break;

        case GrantFactionControl: {
            auto upd = boost::dynamic_pointer_cast<GrantFactionControlMessage>(update);
            if (upd->data1 == game_view->player->id) {
                Faction::pointer faction = game->factions.get(upd->data2);
                game_view->player->grant_control(faction, upd->data3);
            }
        } break;

        case TurnBegin: {
            auto upd = boost::dynamic_pointer_cast<TurnBeginMessage>(update);
            std::ostringstream ss;
            ss << "Day " << upd->data;
            game_view->messages.push_back(InfoMessage(ss.str()));
        } break;

        case TurnEnd: {
        } break;

        case DoBattle: {
            game_view->fix_stack_views();
        } break;

        default:
            break;
    }
}
