#ifndef CHAT_H
#define CHAT_H

#include "hexutil/messaging/receiver.h"

#include "hexav/ui/ui.h"


namespace hex {

class Resources;
class Graphics;
class MessageReceiver;

class ChatWindow: public UiWindow {
public:
    ChatWindow(int width, int height, Graphics *graphics, MessageReceiver *dispatcher);

    virtual bool receive_keyboard_event(SDL_Event *evt);
    virtual void draw(const UiContext& context);

    bool keypress(SDL_Keycode key);
    bool type(SDL_TextInputEvent *evt);
    void add_to_history(const std::string& line);

private:
    Graphics *graphics;
    MessageReceiver *dispatcher;
    bool open;
    std::string chat_line;
    std::vector<std::string> chat_history;
};

class ChatUpdater: public MessageReceiver {
public:
    ChatUpdater(ChatWindow *chat_window);
    void receive(Message *update);

private:
    ChatWindow *chat_window;
};

};

#endif
