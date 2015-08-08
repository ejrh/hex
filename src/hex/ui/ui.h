#ifndef UI_H
#define UI_H

class UiWindow {
public:
    UiWindow(int x, int y, int width, int height);
    virtual ~UiWindow();

    virtual bool receive_event(SDL_Event *evt);
    virtual bool contains(int px, int py);
    virtual void draw();

public:
    int x, y;
    int width, height;
};

class UiLoop {
public:
    UiLoop(unsigned int frame_time);

    void add_window(UiWindow *window);
    UiWindow *find_window(int px, int py);
    void update();
    void run();

public:
    std::vector<UiWindow *> windows;
    unsigned int frame_time;
    bool running;

    UiWindow *current_window;
    int down_pos_x, down_pos_y;
    UiWindow *dragging_window;
    unsigned int last_tick;
};

extern Uint32 drag_event_type;

#endif
