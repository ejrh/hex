#ifndef PALETTE_H
#define PALETTE_H

#include "hexav/ui/ui.h"


namespace hex {

class Game;
class GameView;
class LevelWindow;
class EditorWindow;
class Brush;


class PaletteWindow: public UiWindow {
public:
    PaletteWindow(Game *game, GameView *view, LevelWindow *level_window, EditorWindow *editor_window);

    void create_brush_buttons();

    void show();
    void hide();
    void toggle();

    bool receive_mouse_event(SDL_Event *evt, int x, int y);
    bool receive_keyboard_event(SDL_Event *evt);
    void draw(const UiContext& context);

    static const int palette_width = 160;

private:
    Game *game;
    GameView *view;
    LevelWindow *level_window;
    EditorWindow *editor_window;
    Brush *brush;
};

};

#endif

