#ifndef GENERATOR_H
#define GENERATOR_H

class Updater;
class Game;

class Generator {
public:
    Generator():
        height_scale(8.0f), height_power(1.5f), sea_level(-2.0f), hill_level(0.0f), mountain_level(4.8f), hill_culling(4), mountain_culling(2),
        seed(rand())
    { }
    void create_game(Updater& updater);

private:
    void create_level(Updater& updater);
    void create_factions(Updater& updater);
    void create_unit_stacks(Updater& updater);
    void create_towers(Updater& updater);

public:
    float height_scale;
    float height_power;
    float sea_level;
    float hill_level;
    float mountain_level;
    int hill_culling;
    int mountain_culling;

private:
    unsigned int seed;
    Game *game;
};

#endif
