#ifndef UI_H
#define UI_H

#include "hex/graphics/graphics.h"

enum UiWindowFlags {
    WindowIsVisible = 1,
    WindowIsActive = 2,
    WindowWantsMouseEvents = 4,
    WindowWantsKeyboardEvents = 8,
    WindowWantsGlobalEvents = 16
};

inline UiWindowFlags operator|(UiWindowFlags a, UiWindowFlags b) {
    return static_cast<UiWindowFlags>(static_cast<int>(a) | static_cast<int>(b));
}

class UiContext {
public:
    UiContext(Graphics *graphics, int translate_x, int translate_y):
            graphics(graphics), translate_x(translate_x), translate_y(translate_y) { }

    UiContext get_subcontext(int x, int y, int width, int height) const {
        UiContext new_context(graphics, translate_x + x, translate_y + y);
        return new_context;
    }

    void blit(Image *im, int x, int y) const {
        graphics->blit(im, translate_x + x, translate_y + y);
    }

    void draw_rectangle(Uint8 R, Uint8 G, Uint8 B, int x, int y, int w, int h) const {
        graphics->draw_rectangle(R, G, B, translate_x + x, translate_y + y, w, h);
    }

    void fill_rectangle(Uint8 R, Uint8 G, Uint8 B, int x, int y, int w, int h) const {
        graphics->fill_rectangle(R, G, B, translate_x + x, translate_y + y, w, h);
    }

public:
    Graphics *graphics;
    int translate_x, translate_y;
};

class UiWindow {
public:
    typedef std::unique_ptr<UiWindow> pointer;

    UiWindow(int x, int y, int width, int height, UiWindowFlags flags);
    virtual ~UiWindow();

    virtual bool receive_mouse_event(SDL_Event *evt, int x, int y);
    virtual bool receive_keyboard_event(SDL_Event *evt);
    virtual void receive_global_event(SDL_Event *evt);
    virtual void draw(const UiContext& context);

    bool contains(int px, int py);
    UiWindow *find_child(int px, int py);
    void add_child(UiWindow *child);
    void remove_child(UiWindow *child);
    void set_flag(UiWindowFlags flag) { flags = flags | flag; }
    void clear_flag(UiWindowFlags flag) { flags = flags & ~flag; }

public:
    UiWindow *parent;
    int x, y;
    int width, height;
    std::vector<UiWindow::pointer> children;
    unsigned int flags;
};

class UiLoop {
public:
    UiLoop(Graphics *graphics, unsigned int frame_time);

    void set_root_window(UiWindow *window);
    void update();
    void run();

private:
    bool deliver_event(UiWindow& window, SDL_Event *event, int offset_x, int offset_y);
    bool deliver_event_to_children(UiWindow& window, SDL_Event *event, int offset_x, int offset_y);
    void draw_window(UiWindow& window, const UiContext& context);
    void draw_children(UiWindow& window, const UiContext& context);

public:
    Graphics *graphics;
    unsigned int frame_time;

    UiWindow::pointer root;
    bool running;

    int down_pos_x, down_pos_y;
    UiWindow *dragging_window;
    unsigned int last_tick;
};

extern Uint32 drag_event_type;

#endif
