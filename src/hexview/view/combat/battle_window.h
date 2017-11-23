#ifndef BATTLE_WINDOW_H
#define BATTLE_WINDOW_H

#include "hexav/ui/ui.h"


class ViewResources;
class Graphics;
class BattleView;
class UnitRenderer;

class BattleWindow: public UiWindow {
public:
    BattleWindow(int x, int y, int width, int height, ViewResources *resources, Graphics *graphics, BattleView *view, UnitRenderer *renderer);
    void draw(const UiContext& context);
    void draw_stack(int stack_num);

private:
    ViewResources *resources;
    Graphics *graphics;
    BattleView *view;
    UnitRenderer *renderer;

    ImageSeries target_images;
};


#endif
