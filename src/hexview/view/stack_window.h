#ifndef STACK_WINDOW_H
#define STACK_WINDOW_H

#include "hexav/ui/ui.h"


class ViewResources;
class Graphics;
class GameView;
class LevelView;
class UnitRenderer;
class UnitInfoWindow;

class StackWindow: public UiWindow {
public:
    StackWindow(int x, int y, int width, int height, ViewResources *resources, Graphics *graphics, GameView *view, UnitRenderer *renderer, UnitInfoWindow *unit_info_window);
    bool receive_mouse_event(SDL_Event *evt, int x, int y);
    void draw(const UiContext& context);

    static const int units_across = 4;
    static const int units_down = 3;
    static const int unit_width = 52;
    static const int unit_height = 52;
    static const int padding = 4;
    static const int border = 8;
    static const int window_width = units_across * unit_width + (units_across - 1) * padding + 2 * border;
    static const int window_height = units_down * unit_height + (units_down - 1) * padding + 2 * border;

private:
    ViewResources *resources;
    Graphics *graphics;
    GameView *view;
    UnitRenderer *renderer;
    UnitInfoWindow *unit_info_window;

    std::vector<SDL_Rect> unit_rectangles;
};

#endif
