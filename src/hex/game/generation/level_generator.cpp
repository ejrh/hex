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
#include "hex/messaging/updater.h"

static std::string type(Tile& tile) {
    std::vector<std::string> parts;
    boost::split(parts, tile.type->name, boost::is_any_of("_"));
    return parts[0];
}

static std::string subtype(Tile& tile) {
    std::vector<std::string> parts;
    boost::split(parts, tile.type->name, boost::is_any_of("_"));
    if (parts.size() > 1)
        return parts[1];
    else
        return "";
}

void LevelGenerator::generate_level(int width, int height) {
    BOOST_LOG_TRIVIAL(info) << boost::format("Generating level of size (%d, %d)") % width % height;
    level.width = width;
    level.height = height;
    level.tiles.resize(width, height);

    generate_terrain();
    coalesce_mountains();
    add_forests();
    add_roads();
}

void LevelGenerator::generate_terrain() {
    PerlinNoise noise(3, 3);
    PerlinNoise noise2(6, 6);

    PerlinNoise type_noise1(3, 3);
    PerlinNoise type_noise2(3, 3);

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

            float height = (noise.value(px, py) + noise2.value(py, px)) * generator->height_scale;
            if (height > 0.0)
                height = powf(height, generator->height_power);

            int hill_rand = rand();

            if (height < generator->sea_level)
                tile.type = types["water"];
            else {
                float angle = atan2(type_noise1.value(px, py), type_noise2.value(px, py));
                int sector = (int) ((angle + M_PI) * 5 / (2 * M_PI));
                std::string type_name;
                switch (sector) {
                    case 0: type_name = "grass"; break;
                    case 1: type_name = "desert"; break;
                    case 2: type_name = "steppe"; break;
                    case 3: type_name = "wasteland"; break;
                    default: type_name = "snow"; break;
                }

                if (height > generator->mountain_level) {
                    type_name += "_mountain1";
                } else if (height > generator->hill_level && hill_rand % 2) {
                    type_name += "_hill1";
                }

                tile.type = types[type_name];
                if (!tile.type) {
                    throw Error() << boost::format("Unknown tile type: %s") % type_name;
                }
            }
        }
    }
}

void LevelGenerator::coalesce_mountains() {
    BOOST_LOG_TRIVIAL(info) << "Coalescing hills and mountains";
    for (int i = 0; i < level.height; i++) {
        for (int j = 0; j < level.width; j++) {
            Point tile_pos(j, i);
            Tile& tile = level.tiles[tile_pos];
            std::string tile_type = type(tile);
            std::string tile_subtype = subtype(tile);

            int mountain_rand = rand();
            int hill_rand = rand();

            // Hills can be coalesced in two patterns:
            //       H   and   H
            //     h             h
            int left_rand = rand();
            if (tile_subtype == "hill1") {
                bool left = left_rand % 2 == 0;
                int dir = left ? 4 : 2;
                Point neighbour_pos;
                get_neighbour(tile_pos, dir, &neighbour_pos);
                if (!level.contains(neighbour_pos))
                    continue;
                Tile& neighbour = level.tiles[neighbour_pos];
                if (subtype(neighbour) == "hill1") {
                    tile.type = types[tile_type + (left ? "_hill2" : "_hill3")];
                    neighbour.type = types[tile_type + "_hill0"];
                    tile_subtype = subtype(tile);
                }
            }

            // Mountains can be coalesced as follows:
            //       M
            //     m   m
            //       m
            if (tile_subtype == "mountain1") {
                Point neighbour_pos[6];
                get_neighbours(tile_pos, neighbour_pos);
                if (!level.contains(neighbour_pos[2]) || !level.contains(neighbour_pos[3]) || !level.contains(neighbour_pos[4]))
                    continue;
                Tile& neighbour2 = level.tiles[neighbour_pos[2]];
                Tile& neighbour3 = level.tiles[neighbour_pos[3]];
                Tile& neighbour4 = level.tiles[neighbour_pos[4]];
                if (subtype(neighbour2) != "mountain1" || subtype(neighbour3) != "mountain1" || subtype(neighbour4) != "mountain1")
                    continue;

                tile.type = types[tile_type + "_mountain2"];
                neighbour2.type = types[tile_type + "_mountain0"];
                neighbour3.type = types[tile_type + "_mountain0"];
                neighbour4.type = types[tile_type + "_mountain0"];
                tile_subtype = subtype(tile);
            }

            // Mountains can further be coalesced:
            //          M
            //        m   m
            //      m   m   m
            //        m   m
            //          m
            if (tile_subtype == "mountain2") {
                Point neighbour_pos[6];
                get_neighbours(tile_pos, neighbour_pos);
                Point neighbour_pos_l[6];
                get_neighbours(neighbour_pos[4], neighbour_pos_l);
                Point neighbour_pos_r[6];
                get_neighbours(neighbour_pos[2], neighbour_pos_r);
                Point neighbour_pos_b[6];
                get_neighbours(neighbour_pos[3], neighbour_pos_b);
                if (!level.contains(neighbour_pos_b[3]) || !level.contains(neighbour_pos_l[4]) || !level.contains(neighbour_pos_r[2]))
                    continue;
                Tile& neighbourl4 = level.tiles[neighbour_pos_l[4]];
                Tile& neighbourb4 = level.tiles[neighbour_pos_b[4]];
                Tile& neighbourb3 = level.tiles[neighbour_pos_b[3]];
                Tile& neighbourb2 = level.tiles[neighbour_pos_b[2]];
                Tile& neighbourr2 = level.tiles[neighbour_pos_r[2]];
                if (subtype(neighbourl4) != "mountain1" || subtype(neighbourb4) != "mountain1" || subtype(neighbourb3) != "mountain1"
                        || subtype(neighbourb2) != "mountain1" || subtype(neighbourr2) != "mountain1")
                    continue;

                tile.type = types[tile_type + "_mountain3"];
                neighbourl4.type = types[tile_type + "_mountain0"];
                neighbourb4.type = types[tile_type + "_mountain0"];
                neighbourb3.type = types[tile_type + "_mountain0"];
                neighbourb2.type = types[tile_type + "_mountain0"];
                neighbourr2.type = types[tile_type + "_mountain0"];
            }

            if (tile_subtype == "mountain1" && mountain_rand % generator->mountain_culling) {
                tile.type = types[tile_type];
            }

            if (tile_subtype == "hill1" && hill_rand % generator->hill_culling) {
                tile.type = types[tile_type];
            }
        }
    }
}

void LevelGenerator::add_forests() {
    PerlinNoise forest_noise(6, 6);

    BOOST_LOG_TRIVIAL(info) << "Adding forests";
    for (int i = 0; i < level.height; i++) {
        for (int j = 0; j < level.width; j++) {
            float px = (float) j / level.width;
            float py = (float) i / level.height;
            if (j % 2 == 1)
                py += 0.5f / level.height;

            float forest_value = forest_noise.value(px, py);

            Point tile_pos(j, i);
            Tile& tile = level.tiles[tile_pos];
            std::string tile_type = type(tile);

            int forest_rand = rand();
            if (forest_value > 0.0 && tile.type->name == tile_type && forest_rand % 4 != 0) {
                std::string forest_type = tile_type + ((forest_rand % 3 == 0) ? "_dead_forest" : "_forest");
                if (types.find(forest_type) != types.end())
                    tile.type = types[forest_type];
            }
        }
    }
}

void LevelGenerator::add_roads() {
    BOOST_LOG_TRIVIAL(info) << "Adding roads";
    for (int i = 0; i < 100; i++) {
        Point start_pos(rand() % level.width, rand() % level.height);
        Point end_pos(start_pos.x + rand() % 10 - rand() % 10, start_pos.y + rand() % 10 - rand() % 10);
        if (!level.contains(end_pos))
            continue;
        MovementModel movement(&level);
        Pathfinder pathfinder(&level, &movement);
        UnitStack::pointer party = boost::make_shared<UnitStack>(start_pos, Faction::pointer());
        Unit::pointer unit = boost::make_shared<Unit>();
        unit->type = boost::make_shared<UnitType>();
        unit->properties[Walking] = 1;
        party->units.push_back(unit);
        pathfinder.start(*party, start_pos, end_pos);
        pathfinder.complete();
        Path path;
        pathfinder.build_path(path);
        path.resize(30);
        for (Path::iterator iter = path.begin(); iter != path.end(); iter++) {
            Tile& tile = level.tiles[*iter];
            if (tile.type->has_property(Roadable) && rand() % 6 != 0) {
                Point neighbours[6];
                get_neighbours(*iter, neighbours);
                bool too_many_roads = false;
                for (int i = 0; i < 6; i++) {
                    if (level.tiles[neighbours[i]].has_property(Road) && level.tiles[neighbours[(i+1) % 6]].has_property(Road))
                        too_many_roads = true;
                }
                if (!too_many_roads) {
                    std::string tile_type = type(tile);
                    std::string road_type = tile_type + "_road";
                    tile.type = types[road_type];
                }
            }
        }
    }
}
