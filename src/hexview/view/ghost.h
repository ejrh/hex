#ifndef GHOST_H
#define GHOST_H

#include "hexview/view/view.h"


namespace hex {

class Ghost {
public:
    Ghost(GameView *view, UnitPainter *unit_painter, UnitStack::pointer& stack, const IntSet selected_units);
    ~Ghost();

    void set_target(Point target);
    void retire();
    void update(unsigned int update_ms);

public:
    bool arrived_at_target;
    bool finished;
    GameView *view;

private:
    UnitPainter *unit_painter;
    UnitStackView::pointer stack_view;
    Point position;
    Point target_position;
    unsigned int progress;

    friend class GameView;
    friend class LevelView;
    friend class LevelWindow;
    friend class MapWindow;
    friend class ViewUpdater;
};

};

#endif
