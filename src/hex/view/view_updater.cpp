#include "common.h"

#include "hex/basics/hexgrid.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/messaging/message.h"
#include "hex/view/player.h"
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
            std::vector<std::string>& tile_data = upd->data2;
            for (unsigned int i = 0; i < tile_data.size(); i++) {
                Point tile_pos(offset.x + i, offset.y);
                if (!game->level.contains(tile_pos)) {
                    continue;
                }

                std::string& tile_type_name = tile_data[i];
                TileType *tile_type = game->tile_types[tile_type_name];
                TileView& tile_view = game_view->level_view.tile_views[tile_pos];
                TileViewDef *view_def = resources->get_tile_view_def(tile_type->name);
                tile_view.view_def = view_def;
                tile_view.variation = rand();
                tile_view.phase = rand();
            }

            for (unsigned int i = 0; i < tile_data.size(); i++) {
                Point tile_pos(offset.x + i, offset.y);
                if (!game->level.contains(tile_pos)) {
                    continue;
                }

                TileView& tile_view = game_view->level_view.tile_views[tile_pos];
                TileViewDef *view_def = tile_view.view_def;

                for (int j = 0; j < 3; j++) {
                    int dir = (j + 5) % 6;
                    Point neighbour;
                    get_neighbour(tile_pos, dir, &neighbour);
                    if (game->level.contains(neighbour) && game_view->level_view.tile_views[neighbour].view_def != view_def) {
                        tile_view.transition[j] = choose_image(view_def->transitions[j], tile_view.phase * (j + 2) / 1000);
                    } else {
                        tile_view.transition[j] = NULL;
                    }
                }
            }
        } break;

        case CreateTileType:
        case CreateUnitType:
            // Ignore
            break;

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

        case GrantFactionView: {
            boost::shared_ptr<GrantFactionViewMessage> upd = boost::dynamic_pointer_cast<GrantFactionViewMessage>(update);
            if (upd->data1 == game_view->player->id) {
                Faction *faction = game->get_faction(upd->data2);
                game_view->player->grant_view(faction, upd->data3);
            }
        } break;

        case GrantFactionControl: {
            boost::shared_ptr<GrantFactionControlMessage> upd = boost::dynamic_pointer_cast<GrantFactionControlMessage>(update);
            if (upd->data1 == game_view->player->id) {
                Faction *faction = game->get_faction(upd->data2);
                game_view->player->grant_control(faction, upd->data3);
            }
        } break;

        case TurnEnd: {
        } break;

        case UnitMove: {
            boost::shared_ptr<UnitMoveMessage> upd = boost::dynamic_pointer_cast<UnitMoveMessage>(update);

            UnitStack *stack = game->get_stack(upd->data1);
            if (stack == NULL) {
                game_view->unit_stack_views.erase(upd->data1);
                game_view->selected_stack = NULL;
                game_view->set_drawn_path(Path());
            }

            if (upd->data4 == 0) {
                UnitStack *stack = game->get_stack(upd->data1);
                UnitStackView *stack_view = &game_view->unit_stack_views[stack->id];
                stack_view->path = Path();
                if (game_view->selected_stack == stack) {
                    game_view->set_drawn_path(stack_view->path);
                }
                if (stack_view->view_def != NULL && upd->data3.size() > 1) {
                    Ghost ghost(stack, upd->data3, 0);
                    game_view->ghosts.push_back(ghost);
                    stack_view->moving = true;
                    if (game_view->player->has_view(ghost.stack->owner)) {
                        game_view->update_visibility();
                    }
                }
            } else {
                UnitStack *stack = game->get_stack(upd->data4);
                UnitStackView *stack_view = &game_view->unit_stack_views[stack->id];
                if (stack_view->view_def != NULL && upd->data3.size() > 1) {
                    Ghost ghost(stack, upd->data3, 0);
                    game_view->ghosts.push_back(ghost);
                    //TODO target stack should be visible but not movable
                    //stack_view->moving = true;
                }
            }
        } break;

        default:
            break;
    }
}
