#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/messaging/message.h"

#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_updater.h"
#include "hex/game/combat/combat.h"


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
        case ClearGame: {
            game->level.resize(0, 0);
            game->factions.clear();
            game->stacks.clear();
        } break;

        case SetLevel: {
            auto upd = boost::dynamic_pointer_cast<SetLevelMessage>(update);
            game->level.resize(upd->data1, upd->data2);
        } break;

        case SetLevelData: {
            auto upd = boost::dynamic_pointer_cast<SetLevelDataMessage>(update);
            Point offset = upd->data1;
            CompressableStringVector& tile_data = upd->data2;
            game->set_level_data(offset, tile_data.data);
        } break;

        case CreateTileType: {
            auto upd = boost::dynamic_pointer_cast<CreateTileTypeMessage>(update);
            game->create_tile_type(upd->data);
        } break;

        case CreateUnitType: {
            auto upd = boost::dynamic_pointer_cast<CreateUnitTypeMessage>(update);
            game->create_unit_type(upd->data);
        } break;

        case CreateFaction: {
            auto upd = boost::dynamic_pointer_cast<CreateFactionMessage>(update);
            game->create_faction(upd->data1, upd->data2, upd->data3);
        } break;

        case CreateStack: {
            auto upd = boost::dynamic_pointer_cast<CreateStackMessage>(update);
            game->create_unit_stack(upd->data1, upd->data2, upd->data3);
        } break;

        case CreateUnit: {
            auto upd = boost::dynamic_pointer_cast<CreateUnitMessage>(update);
            game->create_unit(upd->data1, upd->data2);
        } break;

        case TransferUnits: {
            auto upd = boost::dynamic_pointer_cast<TransferUnitsMessage>(update);
            game->transfer_units(upd->data1, upd->data2, upd->data3, upd->data4);
        } break;

        case DestroyStack: {
            auto upd = boost::dynamic_pointer_cast<DestroyStackMessage>(update);
            game->destroy_unit_stack(upd->data);
        } break;

        case CreateStructureType: {
            auto upd = boost::dynamic_pointer_cast<CreateStructureTypeMessage>(update);
            game->create_structure_type(upd->data);
        } break;

        case CreateStructure: {
            auto upd = boost::dynamic_pointer_cast<CreateStructureMessage>(update);
            game->create_structure(upd->data1, upd->data2, upd->data3);
        } break;

        case FactionReady: {
            auto upd = boost::dynamic_pointer_cast<FactionReadyMessage>(update);
            Faction::pointer faction = game->factions.get(upd->data1);
            if (faction) {
                faction->ready = upd->data2;
            }
        } break;

        case TurnBegin: {
            auto upd = boost::dynamic_pointer_cast<TurnBeginMessage>(update);
            game->begin_turn(upd->data);
        } break;

        case TurnEnd: {
            game->end_turn();
        } break;

        case DoBattle: {
            auto upd = boost::dynamic_pointer_cast<DoBattleMessage>(update);
            int attacker_id = upd->data1;
            Point attacking_point = game->stacks.get(attacker_id)->position;
            Point attacked_point = upd->data2;
            std::vector<Move>& moves = upd->data3;
            Battle battle(game, attacked_point, attacking_point, moves);
            battle.replay();
            battle.commit();
        } break;

        default:
            break;
    }
}
