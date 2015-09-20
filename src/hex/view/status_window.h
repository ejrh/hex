#ifndef STATUS_WINDOW_H
#define STATUS_WINDOW_H

#include "hex/ui/ui.h"


class Resources;
class Graphics;
class GameView;

class StatusWindow: public UiWindow {
public:
    StatusWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, GameView *view);
    void draw();

    static const int window_height = 20;

private:
    Resources *resources;
    Graphics *graphics;
    GameView *view;
};

#endif
