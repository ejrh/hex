#ifndef COMBAT_WINDOW_H
#define COMBAT_WINDOW_H

#include "hexav/ui/ui.h"


class ViewResources;
class Graphics;
class CombatView;
class UnitRenderer;

class CombatWindow: public UiWindow {
public:
    CombatWindow(int x, int y, int width, int height, ViewResources *resources, Graphics *graphics, CombatView *view, UnitRenderer *renderer);
    void draw(const UiContext& context);
    void draw_stack(int stack_num);

private:
    ViewResources *resources;
    Graphics *graphics;
    CombatView *view;
    UnitRenderer *renderer;

    ImageSeries target_images;
};


#endif
