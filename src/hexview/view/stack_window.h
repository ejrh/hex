#ifndef STACK_WINDOW_H
#define STACK_WINDOW_H

#include "hexav/ui/ui.h"


namespace hex {

class ViewResources;
class Graphics;
class GameView;
class LevelView;
class UnitRenderer;
class UnitInfoWindow;

class UnitSlotWindow;
class UnitMovesWindow;

class StackWindow: public UiWindow {
public:
    StackWindow(int x, int y, int width, int height, ViewResources *resources, GameView *view, UnitRenderer *renderer, UnitInfoWindow *unit_info_window);

    void update_stack();

    bool receive_ui_event(SDL_Event *evt, UiWindow *control);

    void invert_unit_selection();

    static const int units_across = 4;
    static const int units_down = 3;
    static const int unit_width = 52;
    static const int unit_height = 58;
    static const int padding = 2;
    static const int moves_height = 20;
    static const int border = 8;
    static const int window_width = units_across * unit_width + (units_across - 1) * padding + 2 * border;
    static const int window_height = units_down * unit_height + (units_down - 1) * padding + 2 * border;

private:
    ViewResources *resources;
    GameView *view;
    UnitRenderer *renderer;
    UnitInfoWindow *unit_info_window;

    std::vector<UnitSlotWindow *> unit_slots;
    std::vector<UnitMovesWindow *> unit_moves;
};

};

#endif
