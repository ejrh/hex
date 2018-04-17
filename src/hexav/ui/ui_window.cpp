#include "common.h"

#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"
#include "hexav/ui/ui.h"


namespace hex {

UiWindow::UiWindow(int x, int y, int width, int height, UiWindowFlags flags, Atom name):
        x(x), y(y), width(width), height(height), flags(flags), name(name),
        parent(nullptr) {
}

UiWindow::~UiWindow() { }

bool UiWindow::receive_mouse_event(SDL_Event *evt, int x, int y) { return false; }

bool UiWindow::receive_keyboard_event(SDL_Event *evt) { return false; }

bool UiWindow::receive_ui_event(SDL_Event *evt, UiWindow *control) { return false; }

void UiWindow::receive_global_event(SDL_Event *evt) { }

void UiWindow::draw(const UiContext& context) {
    context.draw_rectangle(255, 255, 255, 0, 0, width, height);
    context.fill_rectangle(0, 0, 0, 1, 1, width-2, height-2);
    TextFormat tf;
    tf.write_text(context.graphics, name, context.translate_x+4, context.translate_y+4);
}

void UiWindow::initialise_paint(Execution *execution) { }

bool UiWindow::contains(int px, int py) {
    return px >= x && py >= y && px < (x + width) && py < (y + height);
}

UiWindow *UiWindow::find_child(int px, int py) {
    for (auto iter = children.rbegin(); iter != children.rend(); iter++) {
        if ((*iter)->contains(px, py)) {
            return iter->get();
        }
    }
    return NULL;
}

void UiWindow::add_child(UiWindow *child) {
    child->parent = this;
    children.push_back(UiWindow::pointer(child));
}

void UiWindow::remove_child(UiWindow *child) {
    for (auto iter = children.begin(); iter != children.end(); iter++) {
        if (iter->get() == child) {
            children.erase(iter);
            return;
        }
    }
}

UiWindow *UiWindow::get_control(Atom control_name) const {
    for (auto iter = children.begin(); iter != children.end(); iter++) {
        UiWindow *child = iter->get();
        if (child && child->name == control_name)
            return child;
    }

    return nullptr;
}

void UiWindow::set_paint_script(AtomMap<Script>& scripts, const Atom script_name) {
    Script::pointer script = scripts.find(script_name);
    if (script) {
        set_paint_script(script);
    } else {
        BOOST_LOG_TRIVIAL(error) << "Window paint script not found: " << script_name;
    }
}

void UiWindow::set_paint_script(Script::pointer paint_script) {
    this->paint_script = paint_script;
    this->set_flag(WindowHasPaint);
    this->needs_repaint = true;
}

};
