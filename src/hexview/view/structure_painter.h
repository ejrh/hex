#ifndef STRUCTURE_PAINTER_H
#define STRUCTURE_PAINTER_H

#include "hexutil/messaging/counter.h"


class Game;
class GameView;
class Resources;
class StructureView;

class StructurePainter {
public:
    StructurePainter(Game *game, GameView *view, Resources *resources):
            game(game), view(view), resources(resources),
            structure_paint_counter("paint.structure") { }

    void repaint(StructureView& structure_view);

private:
    Game *game;
    GameView *view;
    Resources *resources;

    Counter structure_paint_counter;
};

#endif
