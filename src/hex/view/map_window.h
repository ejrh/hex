#ifndef MAP_WINDOW_H
#define MAP_WINDOW_H

#include "hex/basics/point.h"
#include "hex/ui/ui.h"


class GameView;
class LevelWindow;
class Graphics;
class Resources;

class MapWindow: public UiWindow {
public:
    MapWindow(int x, int y, int width, int height, GameView *view, LevelWindow *level_window, Graphics *graphics, Resources *resources);
    ~MapWindow();
    void mouse_to_tile(int x, int y, Point *tile);
    void tile_to_pixel(const Point tile, int *px, int *py);
    void left_click(int x, int y);
    void right_click(int x, int y);

    bool receive_event(SDL_Event *evt);
    void draw();

public:
    GameView *view;

protected:
    LevelWindow *level_window;
    Graphics *graphics;
    Resources *resources;
};

#endif
