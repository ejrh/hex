#ifndef TILE_PAINTER_H
#define TILE_PAINTER_H

#include "hex/basics/point.h"

class Game;
class GameView;
class Resources;

class TilePainter {
public:
    TilePainter(Game *game, GameView *view, Resources *resources);

    void repaint(Point offset, int len);
    void paint_transitions(const Point& tile_pos);
    void paint_roads(const Point& tile_pos);
    void paint_mountains(const Point& tile_pos);

private:
    Game *game;
    GameView *view;
    Resources *resources;
};

#endif
