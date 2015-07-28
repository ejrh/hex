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
            game->level.width = upd->data1;
            game->level.height = upd->data2;
            game->level.tiles.resize(upd->data1, upd->data2);
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

        case CreateStack: {
            boost::shared_ptr<CreateStackMessage> upd = boost::dynamic_pointer_cast<CreateStackMessage>(update);
            game->create_unit_stack(upd->stack_id, upd->position);
        } break;

        case CreateUnit: {
            boost::shared_ptr<CreateUnitMessage> upd = boost::dynamic_pointer_cast<CreateUnitMessage>(update);
            game->create_unit(upd->stack_id, upd->type_name);

            game->level.visibility.rebuild();
            game->level.discovered.update();
        } break;

        case PlayerReady: {
            boost::shared_ptr<PlayerReadyMessage> upd = boost::dynamic_pointer_cast<PlayerReadyMessage>(update);
            game->players[upd->player]->ready = upd->ready;
            if (game->players[0]->ready && game->players[1]->ready) {
                //generate_update(boost::make_shared<TurnEndMessage>(game.turn_number + 1));
            }
        } break;

        case TurnEnd: {
            boost::shared_ptr<TurnEndMessage> upd = boost::dynamic_pointer_cast<TurnEndMessage>(update);
            game->turn_number = upd->turn_number;

            for (int i = 0; i < 2; i++) {
                //generate_update(boost::make_shared<PlayerReadyMessage>(i, false));
            }
        } break;

        case UnitMove: {
            boost::shared_ptr<UnitMoveMessage> upd = boost::dynamic_pointer_cast<UnitMoveMessage>(update);

            // check unit can move
            UnitStack *stack = game->stacks[upd->unit];
            Point& new_pos = upd->path.back();

            game->level.tiles[stack->position].stack = NULL;
            stack->position = new_pos;
            game->level.tiles[stack->position].stack = stack;

            game->level.visibility.rebuild();
            game->level.discovered.update();

            trace("Unit %d moves to %d,%d", upd->unit, new_pos.x, new_pos.y);
        } break;

        default:
            break;
    }
}
