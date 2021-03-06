#ifndef EDITOR_H
#define EDITOR_H

#include "hexav/ui/ui.h"


namespace hex {

class Game;
class GameView;
class LevelWindow;
class Brush;


class EditorWindow: public UiWindow {
public:
    EditorWindow(GameView *view, LevelWindow *level_window);

    bool receive_mouse_event(SDL_Event *evt, int x, int y);

private:
    void paint(Point point);

public:
    Brush *brush;
    int paint_radius;

private:
    GameView *view;
};

};

#endif
