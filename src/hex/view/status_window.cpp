#include "common.h"

#include "hex/game/game.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"
#include "hex/view/status_window.h"
#include "hex/view/view.h"

StatusWindow::StatusWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, GameView *view): UiWindow(x, y, width, height), resources(resources), graphics(graphics), view(view) {
}

void StatusWindow::draw() {
    graphics->fill_rectangle(50,50,100, x, y, width, height);
    if (view->debug_mode) {
        TextFormat tf(graphics, SmallFont10, false, 255,255,255);
        tf.write_text("Debug", x + width - 42, y + 4);
    }
}
