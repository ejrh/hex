#ifndef UNIT_INFO_WINDOW_H
#define UNIT_INFO_WINDOW_H

#include "hex/ui/ui.h"


class Resources;
class Graphics;
class GameView;

class UnitInfoWindow: public UiWindow {
public:
    UnitInfoWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, GameView *view);
    bool contains(int px, int py);
    bool receive_event(SDL_Event *evt);
    void draw();

    void open(Unit::pointer current_unit);
    void close();

    static const int unit_info_window_width = 400;
    static const int unit_info_window_height = 200;

private:
    Resources *resources;
    Graphics *graphics;
    GameView *view;
    bool is_open;
    Unit::pointer current_unit;
};

#endif
