#include "common.h"

#include "hex/noise.h"
#include "hex/basics/hexgrid.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/game/movement/movement.h"
#include "hex/messaging/updater.h"

void generate_level(Level &level, TileTypeMap& types) {
    PerlinNoise noise(5, 5);
    PerlinNoise noise2(10, 10);

    for (int i = 0; i < level.height; i++) {
        for (int j = 0; j < level.width; j++) {
            Tile& tile = level.tiles[i][j];
            if (i == 0 || i == level.height - 1 || j == 0 || j == level.width - 1) {
                tile.type = types["outside"];
                continue;
            }

            float px = (float) j / level.width;
            float py = (float) i / level.height;
            if (j % 2 == 1)
                py += 0.5f / level.height;

            float value = (noise.value(px, py) + noise2.value(py, px))/6.0f;
            if (value < 0.0f)
                tile.type = types["water"];
            else if (value < 0.5f)
                tile.type = types["desert"];
            else {
                tile.type = types["grass"];
                if (value > 1.4f) {
                    tile.type = types["grass_mountain1"];
                } else if (value > 1.2f && rand() % 2) {
                    tile.type = types["grass_hill1"];
                }
            }

            if (tile.type->has_property(Roadable) && rand() % 2)
                tile.road = true;
        }
    }

    // Coalesce hills and mountains
    for (int i = 0; i < level.height; i++) {
        for (int j = 0; j < level.width; j++) {
            Point tile_pos(j, i);
            Tile& tile = level.tiles[tile_pos];
            if (tile.type == types["grass_hill1"]) {
                bool left = rand() % 2 == 0;
                int dir = left ? 5 : 1;
                Point neighbour_pos;
                get_neighbour(tile_pos, dir, &neighbour_pos);
                if (!level.contains(neighbour_pos))
                    continue;
                Tile& neighbour = level.tiles[neighbour_pos];
                if (neighbour.type == types["grass_hill1"]) {
                    tile.type = left ? types["grass_hill3"] : types["grass_hill2"];
                    neighbour.type = types["grass_hill0"];
                }
            } else if (tile.type == types["grass_mountain1"]) {
                Point neighbour_pos[6];
                get_neighbours(tile_pos, neighbour_pos);
                if (!level.contains(neighbour_pos[5]) || !level.contains(neighbour_pos[0]) || !level.contains(neighbour_pos[1]))
                    continue;
                Tile& neighbour5 = level.tiles[neighbour_pos[5]];
                Tile& neighbour0 = level.tiles[neighbour_pos[0]];
                Tile& neighbour1 = level.tiles[neighbour_pos[1]];
                if (neighbour5.type != types["grass_mountain1"] || neighbour0.type != types["grass_mountain1"] || neighbour1.type != types["grass_mountain1"])
                    continue;

                tile.type = types["grass_mountain2"];
                neighbour5.type = types["grass_mountain0"];
                neighbour0.type = types["grass_mountain0"];
                neighbour1.type = types["grass_mountain0"];
            }

            if (tile.type == types["grass_mountain2"]) {
                Point neighbour_pos[6];
                get_neighbours(tile_pos, neighbour_pos);
                Point neighbour_pos_l[6];
                get_neighbours(neighbour_pos[4], neighbour_pos_l);
                Point neighbour_pos_r[6];
                get_neighbours(neighbour_pos[2], neighbour_pos_r);
                if (!level.contains(neighbour_pos[3]) || !level.contains(neighbour_pos_l[5]) || !level.contains(neighbour_pos_r[1]))
                    continue;
                Tile& neighbourl5 = level.tiles[neighbour_pos_l[5]];
                Tile& neighbour4 = level.tiles[neighbour_pos[4]];
                Tile& neighbour3 = level.tiles[neighbour_pos[3]];
                Tile& neighbour2 = level.tiles[neighbour_pos[2]];
                Tile& neighbourr1 = level.tiles[neighbour_pos_r[1]];
                if (neighbourl5.type != types["grass_mountain1"] || neighbour4.type != types["grass_mountain1"] || neighbour3.type != types["grass_mountain1"]
                        || neighbour2.type != types["grass_mountain1"] || neighbourr1.type != types["grass_mountain1"])
                    continue;

                tile.type = types["grass_mountain3"];
                neighbourl5.type = types["grass_mountain0"];
                neighbour4.type = types["grass_mountain0"];
                neighbour3.type = types["grass_mountain0"];
                neighbour2.type = types["grass_mountain0"];
                neighbourr1.type = types["grass_mountain0"];
            }
        }
    }
}

void create_game(Game& game, Updater& updater) {
    int width = 57;
    int height = 48;

    replay_messages("data/tile_types.txt", updater);
    replay_messages("data/unit_types.txt", updater);
    replay_messages("data/structure_types.txt", updater);

    game.level.width = width;
    game.level.height = height;
    game.level.tiles.resize(width, height);
    generate_level(game.level, game.tile_types);

    updater.receive(boost::make_shared<WrapperMessage2<int, int> >(SetLevel, game.level.width, game.level.height));
    for (int i = 0; i < game.level.height; i++) {
        Point origin(0, i);
        std::vector<std::string> data;
        for (int j = 0; j < game.level.width; j++) {
            Tile& tile = game.level.tiles[i][j];
            std::ostringstream data_ss;
            TileType *tile_type = tile.type;
            data_ss << tile_type->name;
            if (tile.road)
                data_ss << "/r";
            data.push_back(data_ss.str());
        }
        updater.receive(boost::make_shared<WrapperMessage2<Point, std::vector<std::string> > >(SetLevelData, origin, data));
    }

    updater.receive(create_message(CreateFaction, 1, "independent", "Independent"));
    updater.receive(create_message(CreateFaction, 2, "orcs", "Orc Hegemony"));
    updater.receive(create_message(CreateFaction, 3, "drow", "Great Drow Empire"));

    updater.receive(create_message(GrantFactionView, 0, 2, true));
    updater.receive(create_message(GrantFactionControl, 0, 2, true));

    updater.receive(create_message(GrantFactionControl, 0, 3, true));

    for (int i = 1; i <= 40; i++) {
        UnitTypeMap::iterator item = game.unit_types.begin();
        std::advance(item, rand() % game.unit_types.size());

        std::map<int, Faction *>::iterator faction_iter = game.factions.begin();
        std::advance(faction_iter, rand() % game.factions.size());
        int faction = faction_iter->second->id;

        MovementModel movement_model(&game.level);
        Point p(-1, -1);
        for (int j = 0; j < 10; j++) {
            int tx = rand() % game.level.width;
            int ty = rand() % game.level.height;
            if (movement_model.admits(item->second, game.level.tiles[ty][tx].type)) {
                p = Point(tx, ty);
                break;
            }
        }

        if (p.x != -1) {
            updater.receive(create_message(CreateStack, i, p, faction));
            int num = (rand() % 5) + 1;
            for (int j = 0; j < num; j++) {
                updater.receive(create_message(CreateUnit, i, item->second->name));
            }
        }
    };

    // Add towers
    for (int i = 0; i < game.level.height; i++) {
        for (int j = 0; j < game.level.width; j++) {
            Point tile_pos(j, i);
            Tile& tile = game.level.tiles[tile_pos];
            if (!tile.type->has_property(Roadable))
                continue;
            Point neighbour_pos[6];
            get_neighbours(tile_pos, neighbour_pos);
            int water_count = 0;
            for (int i = 0; i < 6; i++) {
                if (!game.level.contains(neighbour_pos[i]))
                    continue;
                Tile& neighbour = game.level.tiles[neighbour_pos[i]];
                if (neighbour.type == game.tile_types["water"])
                    water_count++;
            }
            if (water_count >= 3 && rand() % 4 == 0) {
                std::map<int, Faction *>::iterator faction_iter = game.factions.begin();
                std::advance(faction_iter, rand() % game.factions.size());
                int faction = faction_iter->second->id;

                updater.receive(create_message(CreateStructure, tile_pos, "tower", faction));
            }
        }
    }

    game.turn_number = 1;
    updater.receive(create_message(TurnBegin, game.turn_number));
}
