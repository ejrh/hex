#ifndef PALETTE_H
#define PALETTE_H

#include "hexav/ui/ui.h"


namespace hex {

class Game;
class GameView;
class LevelWindow;
class EditorWindow;
class Brush;
class UiTabBar;
class UiButton;

class PaletteWindow: public UiWindow {
public:
    PaletteWindow(Game *game, GameView *view, LevelWindow *level_window, EditorWindow *editor_window);

    void create_terrain_brushes();

    void show();
    void hide();
    void toggle();

    void set_brush(Brush *brush);

    bool receive_keyboard_event(SDL_Event *evt);
    bool receive_ui_event(SDL_Event *evt, UiWindow *control);
    void draw(const UiContext& context);

    static const int palette_width = 160;

    static const int NUM_TABS = 1;

private:
    Game *game;
    LevelWindow *level_window;
    EditorWindow *editor_window;

    UiTabBar *tab_bar;
    UiButton *tab_button[NUM_TABS];
    UiWindow *tab_panel[NUM_TABS];
};

};

#endif

