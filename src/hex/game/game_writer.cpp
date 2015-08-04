#include "common.h"

#include "hex/game/game_writer.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"


void GameWriter::write(Game *game) {
    write_unit_types(game);
    write_tile_types(game);
    write_factions(game);
    write_levels(game);
    write_unit_stacks(game);
}

void GameWriter::write_unit_types(Game *game) {
    for (UnitTypeMap::iterator iter = game->unit_types.begin(); iter != game->unit_types.end(); iter++) {
        emit(create_message(CreateUnitType, *iter->second));
    }
}

void GameWriter::write_tile_types(Game *game) {
    for (TileTypeMap::iterator iter = game->tile_types.begin(); iter != game->tile_types.end(); iter++) {
        emit(create_message(CreateTileType, *iter->second));
    }
}

void GameWriter::write_factions(Game *game) {
    for (std::map<int, Faction *>::iterator iter = game->factions.begin(); iter != game->factions.end(); iter++) {
        Faction *faction = iter->second;
        emit(create_message(CreateFaction, faction->id, faction->type_name, faction->name));
    }
}

void GameWriter::write_levels(Game *game) {
    emit(create_message(SetLevel, game->level.width, game->level.height));
    for (int i = 0; i < game->level.height; i++) {
        Point origin(0, i);
        std::vector<std::string> data;
        for (int j = 0; j < game->level.width; j++) {
            data.push_back(game->level.tiles[i][j].type->name);
        }
        emit(create_message(SetLevelData, origin, data));
    }
}

void GameWriter::write_unit_stacks(Game *game) {
    for (std::map<int, UnitStack *>::iterator iter = game->stacks.begin(); iter != game->stacks.end(); iter++) {
        UnitStack *stack = iter->second;
        emit(create_message(CreateStack, stack->id, stack->position, stack->owner->id));
        for (std::vector<Unit *>::iterator unit_iter = stack->units.begin(); unit_iter != stack->units.end(); unit_iter++) {
            Unit *unit = *unit_iter;
            emit(create_message(CreateUnit, stack->id, unit->type->name));
        }
    }
}

void GameWriter::emit(boost::shared_ptr<Message> message) {
    receiver->receive(message);
}
