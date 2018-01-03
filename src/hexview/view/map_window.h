#ifndef MAP_WINDOW_H
#define MAP_WINDOW_H

#include "hexutil/basics/point.h"

#include "hexav/ui/ui.h"


namespace hex {

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

    bool receive_mouse_event(SDL_Event *evt, int x, int y);
    bool receive_keyboard_event(SDL_Event *evt);
    void draw(const UiContext& context);

private:
    void create_map_image();

public:
    GameView *view;

protected:
    LevelWindow *level_window;
    Graphics *graphics;
    Resources *resources;
    bool dragging;
    Image *map_image;
};

};

#endif
