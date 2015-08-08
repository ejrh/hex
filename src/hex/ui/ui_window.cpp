#include "common.h"

#include "hex/ui/ui.h"


UiWindow::UiWindow(int x, int y, int width, int height): x(x), y(y), width(width), height(height) {
}

UiWindow::~UiWindow() { }

bool UiWindow::receive_event(SDL_Event *evt) {
    return false;
}

bool UiWindow::contains(int px, int py) {
    return px >= x && py >= y && px < (x + width) && py < (y + height);
}

void UiWindow::draw() {
}
