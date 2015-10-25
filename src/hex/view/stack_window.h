#ifndef STACK_WINDOW_H
#define STACK_WINDOW_H

#include "hex/ui/ui.h"


class Resources;
class Graphics;
class LevelView;
class LevelRenderer;

class StackWindow: public UiWindow {
public:
    StackWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, GameView *view, LevelRenderer *renderer);
    bool receive_event(SDL_Event *evt);
    void draw();

    static const int units_across = 4;
    static const int units_down = 3;
    static const int unit_width = 52;
    static const int unit_height = 52;
    static const int padding = 4;
    static const int border = 8;
    static const int window_width = units_across * unit_width + (units_across - 1) * padding + 2 * border;
    static const int window_height = units_down * unit_height + (units_down - 1) * padding + 2 * border;

private:
    Resources *resources;
    Graphics *graphics;
    GameView *view;
    LevelRenderer *renderer;

    std::vector<SDL_Rect> unit_rectangles;
};

#endif
