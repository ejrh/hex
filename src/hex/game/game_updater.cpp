#include "common.h"

#include "hex/basics/error.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_updater.h"
#include "hex/messaging/message.h"


GameUpdater::GameUpdater(Game *game): game(game) {
}

GameUpdater::~GameUpdater() {
}

void GameUpdater::receive(boost::shared_ptr<Message> update) {
    try {
        game->message_id = update->id;
        apply_update(update);
    } catch (const DataError& err) {
        BOOST_LOG_TRIVIAL(error) << "Invalid update received; " << err.what();
    }
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
            game->set_level_data(offset, tile_data);
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
            game->create_faction(upd->data1, upd->data2, upd->data3);
        } break;

        case CreateStack: {
            boost::shared_ptr<CreateStackMessage> upd = boost::dynamic_pointer_cast<CreateStackMessage>(update);
            game->create_unit_stack(upd->data1, upd->data2, upd->data3);
        } break;

        case CreateUnit: {
            boost::shared_ptr<CreateUnitMessage> upd = boost::dynamic_pointer_cast<CreateUnitMessage>(update);
            game->create_unit(upd->data1, upd->data2);
        } break;

        case TransferUnits: {
            boost::shared_ptr<TransferUnitsMessage> upd = boost::dynamic_pointer_cast<TransferUnitsMessage>(update);
            game->transfer_units(upd->data1, upd->data2, upd->data3, upd->data4);
        } break;

        case DestroyStack: {
            boost::shared_ptr<DestroyStackMessage> upd = boost::dynamic_pointer_cast<DestroyStackMessage>(update);
            game->destroy_unit_stack(upd->data);
        } break;

        case CreateStructureType: {
            boost::shared_ptr<CreateStructureTypeMessage> upd = boost::dynamic_pointer_cast<CreateStructureTypeMessage>(update);
            game->create_structure_type(upd->data);
        } break;

        case CreateStructure: {
            boost::shared_ptr<CreateStructureMessage> upd = boost::dynamic_pointer_cast<CreateStructureMessage>(update);
            game->create_structure(upd->data1, upd->data2, upd->data3);
        } break;

        case FactionReady: {
            boost::shared_ptr<FactionReadyMessage> upd = boost::dynamic_pointer_cast<FactionReadyMessage>(update);
            Faction::pointer faction = game->factions.get(upd->data1);
            if (faction) {
                faction->ready = upd->data2;
            }
        } break;

        case TurnBegin: {
            boost::shared_ptr<TurnBeginMessage> upd = boost::dynamic_pointer_cast<TurnBeginMessage>(update);
            game->begin_turn(upd->data);
        } break;

        case TurnEnd: {
            game->end_turn();
        } break;

        default:
            break;
    }
}
