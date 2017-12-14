#include "common.h"

#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"

#include "hexgame/game/game.h"

#include "hexview/view/stack_window.h"
#include "hexview/view/unit_painter.h"
#include "hexview/view/unit_renderer.h"
#include "hexview/view/unit_info_window.h"
#include "hexview/view/view.h"
#include "hexview/view/view_resources.h"


class UnitImage: public UiWindow {
public:
    UnitImage(int x, int y, int width, int height, UiWindowFlags flags, Atom name, ViewResources *resources, UnitPainter *painter, UnitRenderer *renderer):
            UiWindow(x, y, width, height, flags, name),
            resources(resources), painter(painter), renderer(renderer) {
    }

    void draw(const UiContext& context) {
        SDL_Rect saved_clip_rect;
        SDL_Rect clip_rect = { context.translate_x, context.translate_y, width, height };
        saved_clip_rect = context.graphics->set_clip_rect(clip_rect);
        renderer->draw_unit_centered(context.translate_x + width/2, context.translate_y + height/2, unit_view);
        context.graphics->set_clip_rect(saved_clip_rect);
    }

    void set_unit(Unit *unit) {
        unit_view.paint.clear();
        if (unit) {
            unit_view.facing = 2;
            unit_view.phase = 0;
            unit_view.shadow = false;
            unit_view.view_def = resources->get_unit_view_def(unit->type->name);
            painter->repaint(unit_view, *unit);
        }
    }

private:
    ViewResources *resources;
    UnitPainter *painter;
    UnitRenderer *renderer;
    UnitView unit_view;
};


class UnitSlotWindow: public UiButton {
public:
    UnitSlotWindow(int x, int y, int width, int height, Atom name, int slot_num, ViewResources *resources, UnitPainter *painter, UnitRenderer *renderer):
            UiButton(x, y, width, height, "slot", name),
            unit_is_present(false), unit_is_selected(false),
            slot_num(slot_num) {
        unit_image = new UnitImage(4, 4, width-8, height-16, WindowIsVisible, "unit_image", resources, painter, renderer),
        add_child(unit_image);
    }

    void initialise_paint(Execution *execution) {
        execution->variables.set<int>("stack_unit_is_present", unit_is_present);
        execution->variables.set<int>("stack_unit_is_selected", unit_is_selected);
        execution->variables.set<int>("unit_alignment", rand() % 6); //unit->get_property<>(Alignment));
    }

    void set_unit(Unit *unit) {
        if (unit)
            this->unit = unit->shared_from_this();
        else
            this->unit.reset();
        unit_image->set_unit(unit);
    }

/*
            int health = unit.get_property<int>(Health);
            int max_health = unit.type->get_property<int>(Health);
            renderer->draw_health_bar(px - 20, unit_rectangles[i].y, 40, 5, health, max_health);
*/

public:
    bool unit_is_present;
    bool unit_is_selected;

private:
    int slot_num;
    UnitImage *unit_image;
    Unit::pointer unit;
};


class UnitMovesWindow: public UiButton {
public:
    UnitMovesWindow(int x, int y, int width, int height, Atom name, int slot_num):
            UiButton(x, y, width, height, "moves", name),
            unit_is_present(false), unit_is_selected(false),
            slot_num(slot_num) {
    }

    void initialise_paint(Execution *execution) {
        execution->variables.set<int>("stack_unit_is_present", unit_is_present);
        execution->variables.set<int>("stack_unit_is_selected", unit_is_selected);
    }

/*
            std::ostringstream ss;
            ss << unit.get_property<int>(Moves) / MOVE_SCALE;
            px = unit_rectangles[i].x + StackWindow::unit_width;
            py = unit_rectangles[i].y + StackWindow::unit_height;
            tf.write_text(graphics, ss.str(), px, py);
*/

public:
    bool unit_is_present;
    bool unit_is_selected;

private:
    int slot_num;
};


StackWindow::StackWindow(int x, int y, int width, int height, ViewResources *resources, GameView *view, UnitRenderer *renderer, UnitInfoWindow *unit_info_window):
        UiWindow(x, y, width, height, WindowIsVisible|WindowIsActive|WindowWantsUiEvents),
        resources(resources), view(view), renderer(renderer), unit_info_window(unit_info_window) {
    set_paint_script(resources->scripts, "STACK_WINDOW");

    int px = StackWindow::border;
    int py = StackWindow::border;

    unit_slots.resize(MAX_UNITS);
    unit_moves.resize(MAX_UNITS);
    for (unsigned int i = 0; i < MAX_UNITS; i++) {
        unit_slots[i] = new UnitSlotWindow(px, py, StackWindow::unit_width, StackWindow::unit_height, boost::str(boost::format("slot%d") % i), i, resources, &view->unit_painter, renderer);
        add_child(unit_slots[i]);

        unit_moves[i] = new UnitMovesWindow(px, py + StackWindow::unit_height + StackWindow::padding, StackWindow::unit_width, StackWindow::moves_height, boost::str(boost::format("moves%d") % i), i);
        add_child(unit_moves[i]);

        px += StackWindow::unit_width + StackWindow::padding;
        if ((i+1) % 4 == 0) {
            px = StackWindow::border;
            py += StackWindow::unit_height + StackWindow::padding + StackWindow::moves_height + StackWindow::padding;
        }
    }
}

void StackWindow::update_stack() {
    for (unsigned int i = 0; i < MAX_UNITS; i++) {
        unit_slots[i]->unit_is_present = false;
        unit_moves[i]->unit_is_present = false;
        unit_slots[i]->needs_repaint = true;
        unit_moves[i]->needs_repaint = true;
        unit_slots[i]->set_unit(nullptr);
    }

    UnitStack::pointer stack = view->game->stacks.find(view->selected_stack_id);
    if (stack) {
        for (unsigned int i = 0; i < stack->units.size(); i++) {
            unit_slots[i]->unit_is_present = true;
            unit_moves[i]->unit_is_present = true;
            if (view->selected_units.contains(i)) {
                unit_slots[i]->unit_is_selected = true;
                unit_moves[i]->unit_is_selected = true;
            } else {
                unit_slots[i]->unit_is_selected = false;
                unit_moves[i]->unit_is_selected = false;
            }
            unit_slots[i]->set_unit(stack->units[i].get());
        }
    }
}

bool StackWindow::receive_ui_event(SDL_Event *evt, UiWindow *control) {
    if (evt->type == click_event_type && evt->user.code == SDL_BUTTON_LEFT) {
        for (int i = 0; i < MAX_UNITS; i++) {
            if (control == unit_slots[i] && unit_slots[i]->unit_is_present) {
                UnitStack::pointer stack = view->game->stacks.find(view->selected_stack_id);
                unit_info_window->open(stack->units[i]);
            } else if (control == unit_moves[i] && unit_moves[i]->unit_is_present) {
                view->selected_units.toggle(i);
            }
        }
        update_stack();
        return true;
    } else if (evt->type == click_event_type && evt->user.code == SDL_BUTTON_RIGHT) {
        for (int i = 0; i < MAX_UNITS; i++) {
            if (control == unit_moves[i]) {
                invert_unit_selection();
                break;
            }
        }
        update_stack();
        return true;
    }

    return false;
}

void StackWindow::invert_unit_selection() {
    UnitStack::pointer stack = view->game->stacks.find(view->selected_stack_id);
    if (!stack)
        return;
    for (int i = 0; i < stack->units.size(); i++)
        view->selected_units.toggle(i);
    update_stack();
}
