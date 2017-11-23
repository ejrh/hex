#ifndef TRANSITION_PAINT_H
#define TRANSITION_PAINT_H

#include "hexutil/basics/point.h"
#include "hexutil/scripting/scripting.h"

#include "hexav/resources/paint.h"

class Game;
class GameView;
class Resources;


class TransitionPaintExecution: public PaintExecution {
public:
    TransitionPaintExecution(StrMap<Script> *scripts, Resources *resources, Paint *paint,
            Game *game, GameView *view, Point tile_pos):
        PaintExecution(scripts, resources, paint),
        game(game), view(view),
        tile_pos(tile_pos) {
    }

    bool apply_transition(const std::vector<int>& dir_nums, const std::vector<int>& frame_nums);

private:
    Game *game;
    GameView *view;
    Point tile_pos;
    boost::regex pattern_re;

    friend class TransitionMatchInterpreter;
};

void register_transition_paint_interpreters();

#endif
