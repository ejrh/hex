#ifndef LEVEL_GENERATOR_H
#define LEVEL_GENERATOR_H

class Generator;
class Game;

class LevelGenerator {
public:
    LevelGenerator(Generator *generator, Game *game):
            generator(generator),
            game(game),
            level(game->level),
            types(game->tile_types) { }
    void generate_level(int width, int height);

private:
    void generate_terrain();
    void coalesce_mountains();
    void add_forests();
    void add_roads();

private:
    Generator *generator;
    Game *game;
    Level& level;
    std::map<std::string, TileType::pointer>& types;
};

#endif
