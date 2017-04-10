#ifndef UNIT_PAINTER_H
#define UNIT_PAINTER_H

#include "hexutil/messaging/counter.h"


class Game;
class GameView;
class Resources;
class UnitView;
class UnitStackView;

class UnitPainter {
public:
    UnitPainter(Game *game, GameView *view, Resources *resources):
            game(game), view(view), resources(resources),
            unit_paint_counter("paint.unit"), script_error_counter("paint.unit.error") { }

    void repaint(UnitView& unit_view, Unit& unit);
    void repaint(UnitStackView& unit_stack_view, UnitStack& unit_stack);

private:
    Game *game;
    GameView *view;
    Resources *resources;

    Counter unit_paint_counter;
    Counter script_error_counter;
};

#endif
