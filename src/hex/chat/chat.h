#ifndef CHAT_H
#define CHAT_H

#include "hex/messaging/receiver.h"
#include "hex/ui/ui.h"


class Resources;
class Graphics;
class MessageReceiver;

class ChatWindow: public UiWindow {
public:
    ChatWindow(int width, int height, Resources *resources, Graphics *graphics, MessageReceiver *dispatcher);

    virtual bool receive_event(SDL_Event *evt);
    virtual bool contains(int px, int py);
    void draw();

    bool keypress(SDL_Keycode key);
    bool type(SDL_TextInputEvent *evt);
    void add_to_history(const std::string& line);

private:
    Resources *resources;
    Graphics *graphics;
    MessageReceiver *dispatcher;
    bool open;
    std::string chat_line;
    std::vector<std::string> chat_history;
};

class ChatUpdater: public MessageReceiver {
public:
    ChatUpdater(ChatWindow *chat_window);
    void receive(boost::shared_ptr<Message> update);

private:
    ChatWindow *chat_window;
};

#endif
