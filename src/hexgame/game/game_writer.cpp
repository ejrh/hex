#include "common.h"

#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"

#include "hexgame/game/game_writer.h"
#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"


void GameWriter::write(Game *game) {
    write_unit_types(game);
    write_tile_types(game);
    write_feature_types(game);
    write_structure_types(game);
    write_factions(game);
    write_levels(game);
    write_unit_stacks(game);
    write_structures(game);
    write_turn(game);
}

void GameWriter::write_unit_types(Game *game) {
    for (auto iter = game->unit_types.begin(); iter != game->unit_types.end(); iter++) {
        emit(create_message(CreateUnitType, *iter->second));
    }
}

void GameWriter::write_tile_types(Game *game) {
    for (auto iter = game->tile_types.begin(); iter != game->tile_types.end(); iter++) {
        emit(create_message(CreateTileType, *iter->second));
    }
}

void GameWriter::write_feature_types(Game *game) {
    for (auto iter = game->feature_types.begin(); iter != game->feature_types.end(); iter++) {
        emit(create_message(CreateFeatureType, *iter->second));
    }
}

void GameWriter::write_structure_types(Game *game) {
    for (auto iter = game->structure_types.begin(); iter != game->structure_types.end(); iter++) {
        emit(create_message(CreateStructureType, *iter->second));
    }
}

void GameWriter::write_factions(Game *game) {
    for (auto iter = game->factions.begin(); iter != game->factions.end(); iter++) {
        Faction& faction = *iter->second;
        emit(create_message(CreateFaction, faction.id, faction.type_name, faction.name));
    }
}

void GameWriter::write_levels(Game *game) {
    emit(create_message(SetLevel, game->level.width, game->level.height));
    for (int i = 0; i < game->level.height; i++) {
        Point origin(0, i);
        CompressableStringVector type_data;
        CompressableStringVector feature_data;
        for (int j = 0; j < game->level.width; j++) {
            type_data.push_back(game->level.tiles[i][j].type->name);
            feature_data.push_back(game->level.tiles[i][j].feature_type->name);
        }
        emit(create_message(SetLevelData, origin, type_data, feature_data));
    }
}

void GameWriter::write_unit_stacks(Game *game) {
    for (auto iter = game->stacks.begin(); iter != game->stacks.end(); iter++) {
        UnitStack& stack = *iter->second;
        emit(create_message(CreateStack, stack.id, stack.position, stack.owner->id));
        for (auto unit_iter = stack.units.begin(); unit_iter != stack.units.end(); unit_iter++) {
            Unit& unit = **unit_iter;
            emit(create_message(CreateUnit, stack.id, unit.type->name));
        }
    }
}

void GameWriter::write_structures(Game *game) {
    for (int i = 0; i < game->level.height; i++) {
        for (int j = 0; j < game->level.width; j++) {
            Point tile_pos(j, i);
            Structure::pointer structure = game->level.tiles[tile_pos].structure;
            if (!structure)
                continue;
            int owner_id = (structure->owner != NULL) ? structure->owner->id : 0;
            emit(create_message(CreateStructure, tile_pos, structure->type->name, owner_id));
        }
    }
}

void GameWriter::write_turn(Game *game) {
    emit(create_message(TurnBegin, game->turn_number));
}

void GameWriter::emit(const boost::shared_ptr<Message>& message) {
    receiver->receive(message);
}
