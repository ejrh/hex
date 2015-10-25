#include "common.h"

#include "hex/game/game.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"
#include "hex/view/level_renderer.h"
#include "hex/view/stack_window.h"
#include "hex/view/view.h"


StackWindow::StackWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, GameView *view, LevelRenderer *renderer):
        UiWindow(x, y, width, height), resources(resources), graphics(graphics), view(view), renderer(renderer) {
    int px = x + StackWindow::border;
    int py = y + StackWindow::border;

    unit_rectangles.resize(MAX_UNITS);
    for (unsigned int i = 0; i < MAX_UNITS; i++) {
        SDL_Rect r = { px, py, StackWindow::unit_width, StackWindow::unit_height };
        unit_rectangles[i] = r;

        px += StackWindow::unit_width + StackWindow::padding;
        if ((i+1) % 4 == 0) {
            px = x + StackWindow::border;
            py += StackWindow::unit_height + StackWindow::padding;
        }
    }
}

void StackWindow::draw() {
    graphics->fill_rectangle(100,150,100, x, y, width, height);

    UnitStack::pointer stack = view->game->stacks.find(view->selected_stack_id);
    if (stack) {
        TextFormat tf(graphics, SmallFont10, true, 255,255,255);

        for (unsigned int i = 0; i < stack->units.size(); i++) {
            graphics->fill_rectangle(50,50,50, unit_rectangles[i]);
            Unit& unit = *stack->units[i];
            UnitViewDef& view_def = *resources->get_unit_view_def(unit.type->name);
            int px = unit_rectangles[i].x + StackWindow::unit_width / 2;
            int py = unit_rectangles[i].y + StackWindow::unit_height / 2;
            renderer->draw_unit(px, py, unit, view_def);

            std::ostringstream ss;
            ss << unit.moves / MOVE_SCALE;
            px = unit_rectangles[i].x + StackWindow::unit_width;
            py = unit_rectangles[i].y + StackWindow::unit_height;
            tf.write_text(ss.str(), px, py);
        }
    }
}
