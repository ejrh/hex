#ifndef LEVEL_WINDOW_H
#define LEVEL_WINDOW_H

#include "hex/basics/point.h"
#include "hex/ui/ui.h"
#include "hex/view/level_renderer.h"


class LevelView;
class Resources;
class Ghost;

class LevelWindow: public UiWindow {
public:
    LevelWindow(int width, int height, GameView *view, LevelRenderer *level_renderer, Resources *resources);
    ~LevelWindow();
    void set_mouse_position(int x, int y);
    void mouse_to_tile(int x, int y, Point *tile);
    void tile_to_pixel(const Point tile, int *px, int *py);
    void set_position(int x, int y);
    void shift(int xrel, int yrel);
    void left_click(int x, int y);
    void right_click(int x, int y);

    bool receive_mouse_event(SDL_Event *evt, int x, int y);
    bool receive_keyboard_event(SDL_Event *evt);

    void draw(const UiContext& context);
    void draw_level(LevelRenderer::RenderMethod render);
    void draw_ghost(Ghost *ghost);

public:
    GameView *view;
    LevelRenderer *level_renderer;
    bool terrain_only;

protected:
    Resources *resources;
    int shift_x, shift_y;
    int x_spacing, y_spacing;
    bool scroll_up, scroll_down, scroll_left, scroll_right;

    friend class LevelRenderer;
    friend class MapWindow;
};

#endif
