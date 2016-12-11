#include "common.h"

#include "hexutil/basics/hexgrid.h"
#include "hexutil/basics/noise.h"
#include "hexutil/messaging/counter.h"
#include "hexutil/messaging/loader.h"
#include "hexutil/messaging/publisher.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"
#include "hexgame/game/game_updater.h"
#include "hexgame/game/generation/generator.h"
#include "hexgame/game/generation/level_generator.h"
#include "hexgame/game/movement/movement.h"
#include "hexgame/game/movement/pathfinding.h"


int get_random_faction(IntMap<Faction>& factions) {
    auto faction_iter = factions.begin();
    std::advance(faction_iter, rand() % factions.size());
    return faction_iter->second->id;
}

void create_structure(MessageReceiver& updater, Tile& tile, Point tile_pos, std::string structure_type, int faction) {
    if (structure_type != "water_node") {
        CompressableStringVector data;
        data.push_back(type(tile) + "_structure");
        updater.receive(create_message(SetLevelData, tile_pos, data));
    }

    updater.receive(create_message(CreateStructure, tile_pos, structure_type, faction));
}


void Generator::create_game(MessageReceiver& updater) {
    updater.receive(create_message(ClearGame));

    srand(seed);

    game = new Game();
    GameUpdater game_updater(game);
    MessageCounter generation_counter("generator");

    // Wrap the updater in a publisher that will also keep the private game instance up to date
    // with every update emitted by the generator.
    Publisher publisher;
    publisher.subscribe(&game_updater);
    publisher.subscribe(&updater);
    publisher.subscribe(&generation_counter);

    ReceiverMessageLoader loader(publisher);
    loader.load("data/game.txt");

    create_level(publisher);
    create_factions(publisher);
    create_unit_stacks(publisher);
    create_towers(publisher);
    create_mines(publisher);
    create_farms(publisher);
    create_nodes(publisher);

    game->turn_number = 1;
    publisher.receive(create_message(TurnBegin, game->turn_number));

    delete game;
}

void Generator::create_level(MessageReceiver& updater) {
    int width = 57;
    int height = 48;
    LevelGenerator level_generator(this, game);
    level_generator.generate_level(width, height);

    updater.receive(create_message(SetLevel, game->level.width, game->level.height));
    for (int i = 0; i < game->level.height; i++) {
        Point origin(0, i);
        CompressableStringVector data;
        for (int j = 0; j < game->level.width; j++) {
            Tile& tile = game->level.tiles[i][j];
            TileType::pointer tile_type = tile.type;
            data.push_back(tile.type->name);
        }
        updater.receive(create_message(SetLevelData, origin, data));
    }
}

void Generator::create_factions(MessageReceiver& updater) {
    BOOST_LOG_TRIVIAL(info) << "Creating factions";
    updater.receive(create_message(CreateFaction, 1, "independent", "Independent"));
    updater.receive(create_message(CreateFaction, 2, "orcs", "Orc Hegemony"));
    updater.receive(create_message(CreateFaction, 3, "drow", "Great Drow Empire"));
}

void Generator::create_unit_stacks(MessageReceiver& updater) {
    BOOST_LOG_TRIVIAL(info) << "Creating unit stacks";
    for (int i = 1; i <= 50; i++) {
        int faction = get_random_faction(game->factions);
        if (faction == 1)
            continue;

        MovementModel movement_model(&game->level);
        Point p(rand() % game->level.width, rand() % game->level.height);
        Tile& tile = game->level.tiles[p];
        if (tile.stack) {
            continue;
        }

        int num = (rand() % 8) + 1;
        int has_transport = false;
        for (int j = 0; j < num; j++) {
            StrMap<UnitType>::iterator item;
            int attempts = 0;
            do {
                item = game->unit_types.begin();
                std::advance(item, rand() % game->unit_types.size());
                if (attempts++ > 100) {
                    goto max_attempts;
                }
            } while (!movement_model.admits(*item->second, *tile.type));

            if (item->second->has_property(Transport)) {
                if (has_transport) {
                    continue;
                } else {
                    has_transport = true;
                }
            }

            if (j == 0) {
                updater.receive(create_message(CreateStack, i, p, faction));
            }

            updater.receive(create_message(CreateUnit, i, item->second->name));

            max_attempts: ;
        }

        if (p.x != -1) {
        }
    };
}

void Generator::create_towers(MessageReceiver& updater) {
    BOOST_LOG_TRIVIAL(info) << "Placing towers";
    for (int i = 0; i < game->level.height; i++) {
        for (int j = 0; j < game->level.width; j++) {
            Point tile_pos(j, i);
            Tile& tile = game->level.tiles[tile_pos];
            if (!tile.type->has_property(Roadable))
                continue;
            Point neighbour_pos[6];
            get_neighbours(tile_pos, neighbour_pos);
            int water_count = 0;
            for (int i = 0; i < 6; i++) {
                if (!game->level.contains(neighbour_pos[i]))
                    continue;
                Tile& neighbour = game->level.tiles[neighbour_pos[i]];
                if (neighbour.type == game->tile_types.get("water"))
                    water_count++;
            }
            if (water_count >= 3 && rand() % 4 == 0) {
                int faction = get_random_faction(game->factions);

                create_structure(updater, tile, tile_pos, "tower", faction);
            } else if (rand() % 100 == 0) {
                create_structure(updater, tile, tile_pos, "wizard_tower", 0);
            }
        }
    }
}

void Generator::create_mines(MessageReceiver& updater) {
    BOOST_LOG_TRIVIAL(info) << "Placing mines";
    for (int i = 0; i < game->level.height; i++) {
        for (int j = 0; j < game->level.width; j++) {
            Point tile_pos(j, i);
            Tile& tile = game->level.tiles[tile_pos];
            if (subtype(tile) == "" && tile.has_property(Roadable) && rand() % 40 == 0 && !tile.structure) {
                int faction = get_random_faction(game->factions);

                create_structure(updater, tile, tile_pos, "mine", faction);
            }
        }
    }
}

void Generator::create_farms(MessageReceiver& updater) {
    BOOST_LOG_TRIVIAL(info) << "Placing farms";
    for (int i = 0; i < game->level.height; i++) {
        for (int j = 0; j < game->level.width; j++) {
            Point tile_pos(j, i);
            Tile& tile = game->level.tiles[tile_pos];
            if (subtype(tile) == "" && tile.has_property(Roadable) && rand() % 40 == 0 && !tile.structure) {
                int faction = get_random_faction(game->factions);

                create_structure(updater, tile, tile_pos, "farm", faction);
            }
        }
    }
}

void Generator::create_nodes(MessageReceiver& updater) {
    BOOST_LOG_TRIVIAL(info) << "Placing nodes";
    for (int i = 0; i < game->level.height; i++) {
        for (int j = 0; j < game->level.width; j++) {
            Point tile_pos(j, i);
            Tile& tile = game->level.tiles[tile_pos];
            int type_num = rand() % 7;
            if (subtype(tile) == "" && tile.has_property((type_num == 4) ? Swimmable : Roadable) && rand() % 20 == 0 && !tile.structure) {
                int faction = get_random_faction(game->factions);

                std::string type;
                switch (type_num) {
                    case 0: type = "power_node"; break;
                    case 1: type = "life_node"; break;
                    case 2: type = "death_node"; break;
                    case 3: type = "fire_node"; break;
                    case 4: type = "water_node"; break;
                    case 5: type = "earth_node"; break;
                    case 6: type = "air_node"; break;
                }
                create_structure(updater, tile, tile_pos, type, faction);
            }
        }
    }
}
