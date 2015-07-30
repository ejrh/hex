#include "common.h"

#include "hex/basics/error.h"
#include "hex/messaging/serialiser.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"
#include "hex/game/game.h"
#include "hex/game/game_serialisation.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_updater.h"


GameUpdater::GameUpdater(Game *game): game(game) {
}

GameUpdater::~GameUpdater() {
}

void GameUpdater::receive(boost::shared_ptr<Message> update) {
    game->message_id = update->id;
    apply_update(update);
}

void GameUpdater::apply_update(boost::shared_ptr<Message> update) {
    switch (update->type) {
        case SetLevel: {
            boost::shared_ptr<WrapperMessage2<int, int> > upd = boost::dynamic_pointer_cast<WrapperMessage2<int, int> >(update);
            game->level.resize(upd->data1, upd->data2);
        } break;

        case SetLevelData: {
            boost::shared_ptr<WrapperMessage2<Point, std::vector<std::string> > > upd = boost::dynamic_pointer_cast<WrapperMessage2<Point, std::vector<std::string> > >(update);
            Point offset = upd->data1;
            std::vector<std::string>& tile_data = upd->data2;
            for (unsigned int i = 0; i < tile_data.size(); i++) {
                Point tile_pos(offset.x + i, offset.y);
                if (!game->level.contains(tile_pos)) {
                    std::cerr << "Tile coordinate " << tile_pos << " is outside the level" << std::endl;
                    continue;
                }
                std::string& tile_type_name = tile_data[i];
                TileType *tile_type = game->tile_types[tile_type_name];
                game->level.tiles[tile_pos].type = tile_type;
            }
        } break;

        case CreateTileType: {
            boost::shared_ptr<WrapperMessage<TileType> > upd = boost::dynamic_pointer_cast<WrapperMessage<TileType> >(update);
            game->create_tile_type(upd->data);
        } break;

        case CreateUnitType: {
            boost::shared_ptr<WrapperMessage<UnitType> > upd = boost::dynamic_pointer_cast<WrapperMessage<UnitType> >(update);
            game->create_unit_type(upd->data);
        } break;

        case CreateFaction: {
            boost::shared_ptr<CreateFactionMessage> upd = boost::dynamic_pointer_cast<CreateFactionMessage>(update);
            game->create_faction(upd->faction_id, upd->name);
        } break;

        case CreateStack: {
            boost::shared_ptr<CreateStackMessage> upd = boost::dynamic_pointer_cast<CreateStackMessage>(update);
            game->create_unit_stack(upd->stack_id, upd->position, upd->owner);
        } break;

        case CreateUnit: {
            boost::shared_ptr<CreateUnitMessage> upd = boost::dynamic_pointer_cast<CreateUnitMessage>(update);
            game->create_unit(upd->stack_id, upd->type_name);
        } break;

        case FactionReady: {
            boost::shared_ptr<FactionReadyMessage> upd = boost::dynamic_pointer_cast<FactionReadyMessage>(update);
            Faction *faction = game->get_faction(upd->faction);
            if (faction != NULL) {
                faction->ready = upd->ready;
            }
        } break;

        case TurnEnd: {
            boost::shared_ptr<TurnEndMessage> upd = boost::dynamic_pointer_cast<TurnEndMessage>(update);
            game->turn_number = upd->turn_number;
        } break;

        case UnitMove: {
            boost::shared_ptr<UnitMoveMessage> upd = boost::dynamic_pointer_cast<UnitMoveMessage>(update);

            // check unit can move
            UnitStack *stack = game->stacks[upd->unit];
            if (stack == NULL) {
                warn("No stack with id %d", upd->unit);
                return;
            }

            Point& new_pos = upd->path.back();

            game->level.tiles[stack->position].stack = NULL;
            stack->position = new_pos;
            game->level.tiles[stack->position].stack = stack;

            trace("Unit %d moves to %d,%d", upd->unit, new_pos.x, new_pos.y);
        } break;

        default:
            break;
    }
}
