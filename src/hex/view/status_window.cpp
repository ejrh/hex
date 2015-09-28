#include "common.h"

#include "hex/game/game.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"
#include "hex/view/status_window.h"
#include "hex/view/view.h"

StatusWindow::StatusWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, GameView *view): UiWindow(x, y, width, height), resources(resources), graphics(graphics), view(view) {
}

void StatusWindow::draw_faction_readiness() {
    int x_offset = x + width - 100;
    for (std::map<int, FactionView *>::const_iterator iter = view->faction_views.begin(); iter != view->faction_views.end(); iter++) {
        Faction *faction = iter->second->faction;
        FactionViewDef *view_def = iter->second->view_def;
        graphics->fill_rectangle(view_def->r, view_def->g, view_def->b, x_offset, y+2, 8, height-4);
        if (faction->ready) {
            graphics->draw_rectangle(255,255,255, x_offset, y+2, 8, height-4);
        }
        x_offset += 8;
    }
}

void StatusWindow::draw() {
    graphics->fill_rectangle(50,50,100, x, y, width, height);

    draw_faction_readiness();

    if (view->debug_mode) {
        TextFormat tf(graphics, SmallFont10, false, 255,255,255);
        tf.write_text("Debug", x + width - 42, y + 4);
    }
}
