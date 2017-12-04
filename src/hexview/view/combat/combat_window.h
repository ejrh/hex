#ifndef COMBAT_WINDOW_H
#define COMBAT_WINDOW_H

#include "hexav/ui/ui.h"

#include "hexgame/game/game.h"


class ViewResources;
class CombatView;
class UnitRenderer;

class CombatStackWindow: public UiWindow {
public:
    CombatStackWindow(int x, int y, int width, int height, ViewResources *resources, CombatView *view, UnitRenderer *renderer, int stack_num);
    void draw(const UiContext& context);
    void draw_stack(int stack_num);

private:
    ViewResources *resources;
    CombatView *view;
    UnitRenderer *renderer;
    int stack_num;

    ImageSeries target_images;
    int unit_x[MAX_UNITS];
    int unit_y[MAX_UNITS];
};


class CombatWindow: public UiWindow {
public:
    CombatWindow(int x, int y, int width, int height, ViewResources *resources, CombatView *view, UnitRenderer *renderer);

    static const int participant_width = 60;
    static const int participant_height = 50;

    static const int battle_stack_width = participant_width * 4;
    static const int battle_stack_height = participant_height * 3;
    static const int battle_stack_padding = 8;

    static const int combat_window_width = battle_stack_width * 3 + battle_stack_padding * 2;
    static const int combat_window_height = battle_stack_height * 3 + battle_stack_padding * 2;

private:
    CombatStackWindow *stack_windows[7];
};


#endif
