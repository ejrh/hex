#include "common.h"

#include "hex/game/game.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"
#include "hex/view/level_renderer.h"
#include "hex/view/stack_window.h"
#include "hex/view/view.h"


StackWindow::StackWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, GameView *view, LevelRenderer *renderer):
        UiWindow(x, y, width, height), resources(resources), graphics(graphics), view(view), renderer(renderer) {

}

void StackWindow::draw() {
    graphics->fill_rectangle(100,150,100, x, y, width, height);

    UnitStack::pointer stack = view->game->stacks.find(view->selected_stack_id);
    if (stack) {
        int px = x + StackWindow::border;
        int py = y + StackWindow::border;
        int i = 0;

        TextFormat tf(graphics, SmallFont10, true, 255,255,255);

        for (std::vector<Unit::pointer>::iterator iter = stack->units.begin(); iter != stack->units.end(); iter++) {
            graphics->fill_rectangle(50,50,50, px, py, StackWindow::unit_width, StackWindow::unit_height);
            Unit& unit = **iter;
            UnitViewDef& view_def = *resources->get_unit_view_def(unit.type->name);
            renderer->draw_unit(px + StackWindow::unit_width / 2, py + StackWindow::unit_height / 2, unit, view_def);

            std::ostringstream ss;
            ss << unit.moves / MOVE_SCALE;
            tf.write_text(ss.str(), px + StackWindow::unit_width, py + StackWindow::unit_height);

            px += StackWindow::unit_width + StackWindow::padding;
            i++;
            if (i % 4 == 0) {
                px = x + StackWindow::border;
                py += StackWindow::unit_height + StackWindow::padding;
            }
        }
    }
}
