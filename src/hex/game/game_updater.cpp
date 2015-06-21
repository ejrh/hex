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
    apply_update(update);
}

void GameUpdater::apply_update(boost::shared_ptr<Message> update) {
    switch (update->type) {
        case SetLevel: {
            boost::shared_ptr<SetLevelMessage> upd = boost::dynamic_pointer_cast<SetLevelMessage>(update);
            game->level = upd->level;
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

            game->level->tiles[stack->position].stack = NULL;
            stack->position = new_pos;
            game->level->tiles[stack->position].stack = stack;

            trace("Unit %d moves to %d,%d", upd->unit, new_pos.x, new_pos.y);
        } break;

        default: {
            std::cerr << "Unhandled update of type " << update->type << " (" << get_message_type_name(update->type) << ")" << std::endl;
        }
    }
}
