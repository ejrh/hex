#include "common.h"

#include "hexav/ui/ui.h"
#include "hexav/ui/window_painter.h"


namespace hex {

Uint32 user_event_type_base = 0;

Uint32 drag_event_type = 0;
Uint32 focus_event_type = 0;
Uint32 unfocus_event_type = 0;
Uint32 click_event_type = 0;
Uint32 tab_event_type = 0;


static bool is_mouse_event(SDL_Event *event) {
    return event->type >= SDL_MOUSEMOTION && event->type <= SDL_MOUSEWHEEL;
}

static bool is_keyboard_event(SDL_Event *event) {
    return event->type >= SDL_KEYDOWN && event->type <= SDL_TEXTINPUT;
}

static bool is_ui_event(SDL_Event *event) {
    return event->type >= drag_event_type && event->type <= tab_event_type;
}

static void register_user_events() {
    if (user_event_type_base == 0) {
        user_event_type_base = SDL_RegisterEvents(4);

        drag_event_type = user_event_type_base;
        focus_event_type = user_event_type_base+1;
        unfocus_event_type = user_event_type_base+2;
        click_event_type = user_event_type_base+3;
        tab_event_type = user_event_type_base+4;
    }
}

void push_ui_event(Uint32 type, UiWindow *control, int code) {
    if (user_event_type_base == 0) {
        BOOST_LOG_TRIVIAL(error) << "Event pushed before user events registered!";
    }

    SDL_Event event;
    event.type = type;
    event.user.data1 = control;
    event.user.code = code;
    if (!SDL_PushEvent(&event)) {
        BOOST_LOG_TRIVIAL(error) << boost::format("Error pushing SDL event: %s") % SDL_GetError();
    }
}


UiLoop::UiLoop(Graphics *graphics, unsigned int frame_time):
        graphics(graphics), frame_time(frame_time), window_painter(nullptr),
        running(false) {
    register_user_events();
}

UiLoop::UiLoop(Graphics *graphics, unsigned int frame_time, WindowPainter *window_painter):
        graphics(graphics), frame_time(frame_time), window_painter(window_painter),
        running(false) {
    register_user_events();
}

bool UiLoop::deliver_event(UiWindow& window, SDL_Event *event, int offset_x, int offset_y) {
    if (!(window.flags & WindowIsActive))
        return false;

    offset_x += window.x;
    offset_y += window.y;

    bool consumed = deliver_event_to_children(window, event, offset_x, offset_y);
    if (consumed)
        return true;

    if (is_mouse_event(event) && window.flags & WindowWantsMouseEvents) {
        int x = event->motion.x - offset_x;
        int y = event->motion.y - offset_y;
        if (x >= 0 && y >= 0 && x < window.width && y < window.height) {
            if (!(window.flags & WindowHasFocus)) {
                window.set_flag(WindowHasFocus);
                push_ui_event(focus_event_type, &window);
            }
            consumed = window.receive_mouse_event(event, x, y);
            if (event->type == SDL_MOUSEBUTTONDOWN && consumed) {
                dragging_window = &window;
            }
        } else if (window.flags & WindowHasFocus) {
            window.clear_flag(WindowHasFocus);
            push_ui_event(unfocus_event_type, &window);
        }
    } else if (is_keyboard_event(event) && window.flags & WindowWantsKeyboardEvents) {
        consumed = window.receive_keyboard_event(event);
    } else if (is_ui_event(event) && window.flags & WindowWantsUiEvents) {
        consumed = window.receive_ui_event(event, static_cast<UiWindow *>(event->user.data1));
    } else if (window.flags & WindowWantsGlobalEvents) {
        window.receive_global_event(event);
    }
    return consumed;
}

bool UiLoop::deliver_event_to_children(UiWindow& window, SDL_Event *event, int offset_x, int offset_y) {
    for (auto iter = window.children.rbegin(); iter != window.children.rend(); iter++) {
        bool consumed = deliver_event(**iter, event, offset_x, offset_y);
        if (consumed)
            return true;
    }
    return false;
}

void UiLoop::draw_window(UiWindow& window, const UiContext& context) {
    if (window.flags & WindowIsVisible) {
        if (window_painter && window.flags & WindowHasPaint) {
            if (window.needs_repaint) {
                window_painter->repaint_window(&window);
                window.needs_repaint = false;
            }

            window.paint.render(context.translate_x, context.translate_y, 0, context.graphics);
        } else {
            window.draw(context);
        }
        draw_children(window, context);
    }
}

void UiLoop::draw_children(UiWindow& window, const UiContext& context) {
    for (auto iter = window.children.begin(); iter != window.children.end(); iter++) {
        UiWindow& child = **iter;
        UiContext new_context = context.get_subcontext(child.x, child.y, child.width, child.height);
        draw_window(child, new_context);
    }
}

void UiLoop::set_root_window(UiWindow *window) {
    root = UiWindow::pointer(window);
}

void UiLoop::update() {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
        if (root) {
            deliver_event(*root, &evt, 0, 0);
        }
        if (evt.type == SDL_MOUSEBUTTONUP) {
            dragging_window = NULL;
        } else if (evt.type == SDL_MOUSEMOTION && dragging_window != NULL) {
            evt.type = drag_event_type;
            dragging_window->receive_mouse_event(&evt, 0, 0); // TODO pass in position of mouse relative to window
        }
    }

    if (root) {
        UiContext context(graphics, 0, 0);
        draw_window(*root, context);
    }

    unsigned int tick = SDL_GetTicks();
    unsigned int tick_diff = tick - last_tick;
    unsigned int sleep_time = frame_time - tick_diff;
    if (sleep_time > frame_time)
        sleep_time = frame_time;
    SDL_Delay(sleep_time);
    last_tick = tick;
}

void UiLoop::run() {
    down_pos_x = 0;
    down_pos_y = 0;
    dragging_window = NULL;
    last_tick = 0;
    running = true;
    while (running) {
        update();
    }
}

};
