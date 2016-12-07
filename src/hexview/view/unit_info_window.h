#ifndef UNIT_INFO_WINDOW_H
#define UNIT_INFO_WINDOW_H

#include "hexav/ui/ui.h"
#include "hexav/ui/controls.h"


class Resources;
class Graphics;
class GameView;

class UnitInfoWindow: public UiDialog {
public:
    UnitInfoWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, GameView *view);
    bool receive_keyboard_event(SDL_Event *evt);
    void draw(const UiContext& context);

    void open(Unit::pointer current_unit);
    void close();

    static const int unit_info_window_width = 400;
    static const int unit_info_window_height = 400;

private:
    Resources *resources;
    Graphics *graphics;
    GameView *view;
    Unit::pointer current_unit;

    UiTextList *abilities_list;
};

#endif
