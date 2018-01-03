#ifndef LEVEL_GENERATOR_H
#define LEVEL_GENERATOR_H

namespace hex {

class Generator;
class Game;

class LevelGenerator {
public:
    LevelGenerator(Generator *generator, Game *game):
            generator(generator),
            game(game),
            level(game->level),
            types(game->tile_types),
            feature_types(game->feature_types) { }
    void generate_level(int width, int height);

private:
    void generate_terrain();
    void coalesce_mountains();
    void add_rivers();
    void add_forests();
    void add_bridges();
    void add_roads();

private:
    Generator *generator;
    Game *game;
    Level& level;
    std::map<std::string, TileType::pointer>& types;
    std::map<std::string, FeatureType::pointer>& feature_types;
};

};

#endif
