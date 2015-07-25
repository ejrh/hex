#ifndef CHAT_H
#define CHAT_H

#include "hex/ui/ui.h"

class Resources;
class Graphics;
class MessageReceiver;

class ChatWindow: public UiWindow {
public:
    ChatWindow(int width, int height, Resources *resources, Graphics *graphics, MessageReceiver *dispatcher);

    void draw();
    void keypress(SDL_Keycode key);
    void type(SDL_TextInputEvent *evt);
    void add_to_history(const std::string& line);

private:
    Resources *resources;
    Graphics *graphics;
    MessageReceiver *dispatcher;
    bool open;
    std::string chat_line;
    std::vector<std::string> chat_history;
};

#endif
