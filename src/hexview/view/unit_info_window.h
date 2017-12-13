#ifndef UNIT_INFO_WINDOW_H
#define UNIT_INFO_WINDOW_H

#include "hexav/ui/ui.h"
#include "hexav/ui/controls.h"


class Resources;
class Graphics;
class GameView;

class ItemSlotWindow;

#define NUM_ITEM_SLOTS 19

class UnitInfoWindow: public UiWindow {
public:
    UnitInfoWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, GameView *view);
    bool receive_keyboard_event(SDL_Event *evt);
    bool receive_ui_event(SDL_Event *evt, UiWindow *control);
    void draw(const UiContext& context);

    void open(Unit::pointer current_unit);
    void close();

    static const int unit_info_window_width = 420;
    static const int unit_info_window_height = 417;

private:
    Resources *resources;
    Graphics *graphics;
    GameView *view;
    Unit::pointer current_unit;

    UiButton *close_button;
    UiTabBar *tab_bar;
    UiButton *tab_button[3];
    UiWindow *tab_panel[3];
    UiLabel *title;
    UiTextList *abilities_list;

    ItemSlotWindow *item_slots[NUM_ITEM_SLOTS];
};

#endif
