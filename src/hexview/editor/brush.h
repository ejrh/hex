#ifndef BRUSH_H
#define BRUSH_H

#include "hexutil/basics/point.h"

#include "hexview/view/view.h"


namespace hex {

class Brush {
public:
    Brush(Game *game, GameView *view);

    void set_type(const std::string& type_name);
    void next_type();

    void paint_tiles(const Point point);
    bool paint_tile(const Tile& tile, std::string& new_tile_type, std::string& new_feature_type);

public:
    int paint_radius;
    TileType::pointer tile_type;

private:
    Game *game;
    GameView *view;
};

};

#endif

