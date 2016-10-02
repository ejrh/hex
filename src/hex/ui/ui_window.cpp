#include "common.h"

#include "hex/graphics/graphics.h"
#include "hex/ui/ui.h"


UiWindow::UiWindow(int x, int y, int width, int height, UiWindowFlags flags):
        x(x), y(y), width(width), height(height), flags(flags) {
}

UiWindow::~UiWindow() { }

bool UiWindow::receive_mouse_event(SDL_Event *evt, int x, int y) { return false; }

bool UiWindow::receive_keyboard_event(SDL_Event *evt) { return false; }

void UiWindow::receive_global_event(SDL_Event *evt) { }

void UiWindow::draw(const UiContext& context) { }

bool UiWindow::contains(int px, int py) {
    return px >= x && py >= y && px < (x + width) && py < (y + height);
}

UiWindow *UiWindow::find_child(int px, int py) {
    for (std::vector<UiWindow *>::reverse_iterator iter = children.rbegin(); iter != children.rend(); iter++) {
        if ((*iter)->contains(px, py)) {
            return *iter;
        }
    }
    return NULL;
}

void UiWindow::add_child(UiWindow *child) {
    child->parent = this;
    children.push_back(child);
}

void UiWindow::remove_child(UiWindow *child) {
    std::vector<UiWindow *>::iterator iter = std::find(children.begin(), children.end(), child);
    if (iter != children.end()) {
        children.erase(iter);
    }
}
