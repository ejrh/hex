#include "common.h"

#include "hex/chat/chat.h"
#include "hex/game/game_messages.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"


ChatWindow::ChatWindow(int width, int height, Resources *resources, Graphics *graphics, MessageReceiver *dispatcher):
        UiWindow(0, 0, width, height), graphics(graphics), dispatcher(dispatcher), open(false) {
}

void ChatWindow::draw() {
    int first_line = chat_history.size() - 10;
    if (first_line < 0)
        first_line = 0;
    int y_offset = 8;
    TextFormat tf(graphics, SmallFont14, false, 255,255,255, 0,0,0);
    for (unsigned int i = first_line; i < chat_history.size(); i++) {
        tf.write_text(chat_history[i], x + 8, y_offset);
        y_offset += 16;
    }

    if (open) {
        graphics->fill_rectangle(0, 0, 0, 200, 300, graphics->width - 400, 16);
        if (chat_line.size() > 0) {
            TextFormat tf(graphics, SmallFont14, false, 128,255,255, 0,255,255);
            tf.write_text(chat_line, 200, 300);
        }
    }
}

void ChatWindow::keypress(SDL_Keycode key) {
    if (open) {
        if (key == SDLK_RETURN) {
            if (chat_line.size() > 0) {
                dispatcher->receive(boost::make_shared<WrapperMessage<std::string> >(Chat, chat_line));
                chat_line.clear();
            }
            open = false;
        } else if (key == SDLK_BACKSPACE && chat_line.size() > 0) {
            chat_line.erase(chat_line.size() - 1);
        }
    } else {
        if (key == SDLK_RETURN) {
            open = true;
        }
    }
}

void ChatWindow::type(SDL_TextInputEvent *evt) {
    if (!open)
        return;

    chat_line.append(evt->text);
}

void ChatWindow::add_to_history(const std::string& line) {
    chat_history.push_back(line);
}
