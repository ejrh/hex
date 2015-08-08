#ifndef TILE_PAINTER_H
#define TILE_PAINTER_H

#include "hex/basics/point.h"

class Game;
class GameView;
class Resources;

class TilePainter {
public:
    TilePainter(Game *game, GameView *view, Resources *resources);

    void repaint(const Point& tile_pos);
    void repaint(Point offset, int len);

private:
    Game *game;
    GameView *view;
    Resources *resources;
};

#endif
