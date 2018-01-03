#ifndef STATUS_WINDOW_H
#define STATUS_WINDOW_H

#include "hexav/ui/ui.h"


namespace hex {

class Resources;
class Graphics;
class GameView;

class StatusWindow: public UiWindow {
public:
    StatusWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, GameView *view);
    void draw_faction_readiness();
    void draw(const UiContext& context);

    static const int window_height = 20;

private:
    Resources *resources;
    Graphics *graphics;
    GameView *view;
};

};

#endif
