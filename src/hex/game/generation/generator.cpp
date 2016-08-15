#include "common.h"

#include "hex/basics/hexgrid.h"
#include "hex/basics/noise.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_updater.h"
#include "hex/game/generation/generator.h"
#include "hex/game/generation/level_generator.h"
#include "hex/game/movement/movement.h"
#include "hex/game/movement/pathfinding.h"
#include "hex/messaging/loader.h"
#include "hex/messaging/publisher.h"


void Generator::create_game(MessageReceiver& updater) {
    updater.receive(create_message(ClearGame));

    srand(seed);

    game = new Game();
    GameUpdater game_updater(game);

    // Wrap the updater in a publisher that will also keep the private game instance up to date
    // with every update emitted by the generator.
    Publisher publisher;
    publisher.subscribe(&game_updater);
    publisher.subscribe(&updater);

    ReceiverMessageLoader loader(publisher);
    loader.load("data/game.txt");

    create_level(publisher);
    create_factions(publisher);
    create_unit_stacks(publisher);
    create_towers(publisher);

    game->turn_number = 1;
    publisher.receive(create_message(TurnBegin, game->turn_number));

    delete game;
}

void Generator::create_level(MessageReceiver& updater) {
    int width = 57;
    int height = 48;
    LevelGenerator level_generator(this, game);
    level_generator.generate_level(width, height);

    updater.receive(boost::make_shared<WrapperMessage2<int, int> >(SetLevel, game->level.width, game->level.height));
    for (int i = 0; i < game->level.height; i++) {
        Point origin(0, i);
        CompressableStringVector data;
        for (int j = 0; j < game->level.width; j++) {
            Tile& tile = game->level.tiles[i][j];
            TileType::pointer tile_type = tile.type;
            data.push_back(tile.type->name);
        }
        updater.receive(boost::make_shared<SetLevelDataMessage>(SetLevelData, origin, data));
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
        IntMap<Faction>::iterator faction_iter = game->factions.begin();
        std::advance(faction_iter, rand() % game->factions.size());
        int faction = faction_iter->second->id;
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
                IntMap<Faction>::iterator faction_iter = game->factions.begin();
                std::advance(faction_iter, rand() % game->factions.size());
                int faction = faction_iter->second->id;

                updater.receive(create_message(CreateStructure, tile_pos, "tower", faction));
            } else if (rand() % 100 == 0) {
                updater.receive(create_message(CreateStructure, tile_pos, "wizard_tower", 0));
            }
        }
    }
}
