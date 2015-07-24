#ifndef LEVEL_WINDOW_H
#define LEVEL_WINDOW_H

#include "hex/ui/ui.h"

class LevelWindow: public UiWindow {
public:
    LevelWindow(int width, int height, LevelView *level_view, LevelRenderer *level_renderer, Resources *resources);
    ~LevelWindow();
    void set_mouse_position(int x, int y);
    void mouse_to_tile(int x, int y, Point *tile);
    void tile_to_pixel(const Point tile, int *px, int *py);
    void shift(int xrel, int yrel);
    void left_click(int x, int y);
    void right_click(int x, int y);

    void draw();
    void draw_level(LevelRenderer::RenderMethod render);
    void draw_moving_unit();

public:
    LevelView *level_view;
    LevelRenderer *level_renderer;

protected:
    Resources *resources;
    int shift_x, shift_y;
    int x_spacing, y_spacing;

    friend class LevelRenderer;
};

#endif
