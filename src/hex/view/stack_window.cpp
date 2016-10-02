#include "common.h"

#include "hex/game/game.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"
#include "hex/view/stack_window.h"
#include "hex/view/unit_renderer.h"
#include "hex/view/view.h"
#include "hex/view/unit_info_window.h"


StackWindow::StackWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, GameView *view, UnitRenderer *renderer, UnitInfoWindow *unit_info_window):
        UiWindow(x, y, width, height), resources(resources), graphics(graphics), view(view), renderer(renderer), unit_info_window(unit_info_window) {
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

bool StackWindow::receive_event(SDL_Event *evt) {
    UnitStack::pointer stack = view->game->stacks.find(view->selected_stack_id);
    if (!stack)
        return false;

    if (evt->type == SDL_MOUSEBUTTONUP && evt->button.button == SDL_BUTTON_LEFT) {
        for (unsigned int i = 0; i < stack->units.size(); i++) {
            if (rect_contains(unit_rectangles[i], evt->button.x, evt->button.y)) {
                view->selected_units.toggle(i);
                return true;
            }
        }
    }
    if (evt->type == SDL_MOUSEBUTTONUP && evt->button.button == SDL_BUTTON_RIGHT) {
        for (unsigned int i = 0; i < stack->units.size(); i++) {
            if (rect_contains(unit_rectangles[i], evt->button.x, evt->button.y)) {
                Unit& unit = *stack->units[i];
                unit_info_window->open(unit.shared_from_this());
                return true;
            }
        }
    }
    if (evt->type == SDL_MOUSEBUTTONUP && evt->button.button == SDL_BUTTON_RIGHT) {
        for (unsigned int i = 0; i < stack->units.size(); i++) {
            view->selected_units.toggle(i);
        }
        return true;
    }
    return false;
}

void StackWindow::draw() {
    graphics->fill_rectangle(100,150,100, x, y, width, height);

    UnitStack::pointer stack = view->game->stacks.find(view->selected_stack_id);
    if (stack) {
        TextFormat tf(SmallFont10, true, 255,255,255);

        for (unsigned int i = 0; i < stack->units.size(); i++) {
            Unit& unit = *stack->units[i];
            UnitView unit_view;
            unit_view.facing = 2;
            unit_view.phase = 0;
            unit_view.view_def = resources->get_unit_view_def(unit.type->name);
            unit_view.selected = false;
            if (view->selected_units.contains(i)) {
                graphics->fill_rectangle(25,25,25, unit_rectangles[i]);
                unit_view.selected = true;
            } else {
                graphics->fill_rectangle(75,75,75, unit_rectangles[i]);
            }
            int px = unit_rectangles[i].x + StackWindow::unit_width / 2;
            int py = unit_rectangles[i].y + StackWindow::unit_height / 2;
            renderer->draw_unit_centered(px, py, unit_view);

            std::ostringstream ss;
            ss << unit.get_property<int>(Moves) / MOVE_SCALE;
            px = unit_rectangles[i].x + StackWindow::unit_width;
            py = unit_rectangles[i].y + StackWindow::unit_height;
            tf.write_text(graphics, ss.str(), px, py);
        }
    }
}
