#include "common.h"

#include "hex/game/game.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"
#include "hex/view/message_window.h"
#include "hex/view/view.h"


MessageWindow::MessageWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, GameView *view):
        UiWindow(x, y, width, height), resources(resources), graphics(graphics), view(view) {
}

void MessageWindow::draw() {
    graphics->fill_rectangle(100,150,150, x, y, width, height);
    graphics->fill_rectangle(0,0,0, x+4, y+4, width-8, height-8);

    int first_line = view->messages.size() - (height / 12);
    if (first_line < 0)
        first_line = 0;
    int y_offset = y + 8;
    TextFormat tf(graphics, SmallFont10, false, 192,192,192, 0,0,0);
    for (unsigned int i = first_line; i < view->messages.size(); i++) {
        InfoMessage& message = view->messages[i];
        tf.write_text(message.text, x + 8, y_offset);
        y_offset += 12;
    }
}
