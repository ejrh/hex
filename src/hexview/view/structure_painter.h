#ifndef STRUCTURE_PAINTER_H
#define STRUCTURE_PAINTER_H

#include "hexutil/messaging/counter.h"


namespace hex {

class Game;
class GameView;
class Resources;
class StructureView;

class StructurePainter {
public:
    StructurePainter(Game *game, Resources *resources):
            game(game), resources(resources),
            structure_paint_counter("paint.structure"), structure_paint_time("paint.structure.time"), structure_paint_error("paint.structure.error") { }

    void repaint(StructureView& structure_view);

private:
    Game *game;
    Resources *resources;

    Counter structure_paint_counter;
    Counter structure_paint_time;
    Counter structure_paint_error;
};

};

#endif
