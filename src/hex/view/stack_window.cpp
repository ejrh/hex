#include "common.h"

#include "hex/graphics/graphics.h"
#include "hex/game/game.h"
#include "hex/view/view.h"
#include "hex/view/level_renderer.h"
#include "hex/view/stack_window.h"


StackWindow::StackWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, LevelView *level_view, LevelRenderer *renderer):
        UiWindow(x, y, width, height), resources(resources), graphics(graphics), level_view(level_view), renderer(renderer) {

}

void StackWindow::draw() {
    graphics->fill_rectangle(100,150,100, x, y, width, height);

    UnitStack *stack = level_view->selected_stack;
    if (stack != NULL) {
        int px = x + StackWindow::border;
        int py = y + StackWindow::border;
        int i = 0;
        for (std::vector<Unit *>::iterator iter = stack->units.begin(); iter != stack->units.end(); iter++) {
            graphics->fill_rectangle(50,50,50, px, py, StackWindow::unit_width, StackWindow::unit_height);
            Unit *unit = *iter;
            UnitViewDef *view_def = resources->get_unit_view_def(unit->type->name);
            renderer->draw_unit(px+4, py+32, *unit, view_def);
            px += StackWindow::unit_width + StackWindow::padding;
            i++;
            if (i % 4 == 0) {
                px = x + StackWindow::border;
                py += StackWindow::unit_height + StackWindow::padding;
            }
        }
    }
}
