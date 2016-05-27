#ifndef BATTLE_WINDOW_H
#define BATTLE_WINDOW_H

#include "hex/ui/ui.h"


class Resources;
class Graphics;
class BattleView;
class LevelRenderer;

class BattleWindow: public UiWindow {
public:
    BattleWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, BattleView *view, LevelRenderer *renderer);
    bool receive_event(SDL_Event *evt);
    void draw();
    void draw_stack(int stack_num);

private:
    Resources *resources;
    Graphics *graphics;
    BattleView *view;
    LevelRenderer *renderer;
};


#endif
