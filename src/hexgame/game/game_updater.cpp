#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/messaging/message.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"
#include "hexgame/game/game_updater.h"
#include "hexgame/game/combat/combat.h"


GameUpdater::GameUpdater(Game *game): game(game) {
}

GameUpdater::~GameUpdater() {
}

void GameUpdater::receive(Message *update) {
    try {
        game->message_id = update->id;
        apply_update(update);
    } catch (const DataError& err) {
        BOOST_LOG_TRIVIAL(error) << "Invalid update received; " << err.what();
    }
}

void GameUpdater::apply_update(Message *update) {
    switch (update->type) {
        case ClearGame: {
            game->level.resize(0, 0);
            game->factions.clear();
            game->stacks.clear();
        } break;

        case SetLevel: {
            auto upd = dynamic_cast<SetLevelMessage *>(update);
            game->level.resize(upd->data1, upd->data2);
        } break;

        case SetLevelData: {
            auto upd = dynamic_cast<SetLevelDataMessage *>(update);
            Point offset = upd->data1;
            CompressableStringVector& tile_data = upd->data2;
            CompressableStringVector& feature_data = upd->data3;
            game->set_level_data(offset, tile_data.data, feature_data.data);
        } break;

        case CreateTileType: {
            auto upd = dynamic_cast<CreateTileTypeMessage *>(update);
            game->create_tile_type(upd->data);
        } break;

        case CreateFeatureType: {
            auto upd = dynamic_cast<CreateFeatureTypeMessage *>(update);
            game->create_feature_type(upd->data);
        } break;

        case CreateUnitType: {
            auto upd = dynamic_cast<CreateUnitTypeMessage *>(update);
            game->create_unit_type(upd->data);
        } break;

        case CreateFaction: {
            auto upd = dynamic_cast<CreateFactionMessage *>(update);
            game->create_faction(upd->data1, upd->data2, upd->data3);
        } break;

        case CreateStack: {
            auto upd = dynamic_cast<CreateStackMessage *>(update);
            game->create_unit_stack(upd->data1, upd->data2, upd->data3);
        } break;

        case CreateUnit: {
            auto upd = dynamic_cast<CreateUnitMessage *>(update);
            game->create_unit(upd->data1, upd->data2);
        } break;

        case TransferUnits: {
            auto upd = dynamic_cast<TransferUnitsMessage *>(update);
            game->transfer_units(upd->data1, upd->data2, upd->data3, upd->data4);
        } break;

        case DestroyStack: {
            auto upd = dynamic_cast<DestroyStackMessage *>(update);
            game->destroy_unit_stack(upd->data);
        } break;

        case CreateStructureType: {
            auto upd = dynamic_cast<CreateStructureTypeMessage *>(update);
            game->create_structure_type(upd->data);
        } break;

        case CreateStructure: {
            auto upd = dynamic_cast<CreateStructureMessage *>(update);
            game->create_structure(upd->data1, upd->data2, upd->data3);
        } break;

        case FactionReady: {
            auto upd = dynamic_cast<FactionReadyMessage *>(update);
            Faction::pointer faction = game->factions.get(upd->data1);
            if (faction) {
                faction->ready = upd->data2;
            }
        } break;

        case TurnBegin: {
            auto upd = dynamic_cast<TurnBeginMessage *>(update);
            game->begin_turn(upd->data);
        } break;

        case TurnEnd: {
            game->end_turn();
        } break;

        case DoBattle: {
            auto upd = dynamic_cast<DoBattleMessage *>(update);
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
