#include "common.h"

#include "hex/ui/ui.h"

Uint32 drag_event_type = 0;


static bool is_mouse_event(SDL_Event *event) {
    return event->type >= SDL_MOUSEMOTION && event->type <= SDL_MOUSEWHEEL;
}

static bool is_keyboard_event(SDL_Event *event) {
    return event->type >= SDL_KEYDOWN && event->type <= SDL_TEXTINPUT;
}


UiLoop::UiLoop(Graphics *graphics, unsigned int frame_time): graphics(graphics), frame_time(frame_time), running(false) {
    if (drag_event_type == 0)
        drag_event_type = SDL_RegisterEvents(1);
}

bool UiLoop::deliver_event(UiWindow *window, SDL_Event *event, int offset_x, int offset_y) {
    if (!(window->flags & WindowIsActive))
        return false;

    offset_x += window->x;
    offset_y += window->y;

    bool consumed = deliver_event_to_children(window, event, offset_x, offset_y);
    if (consumed)
        return true;

    if (is_mouse_event(event) && window->flags & WindowWantsMouseEvents) {
        int x = event->motion.x - offset_x;
        int y = event->motion.y - offset_y;
        if (x >= 0 && y >= 0 && x < window->width && y < window->height) {
            consumed = window->receive_mouse_event(event, x, y);
            if (event->type == SDL_MOUSEBUTTONDOWN && consumed) {
                dragging_window = window;
            }
        }
    } else if (is_keyboard_event(event) && window->flags & WindowWantsKeyboardEvents) {
        consumed = window->receive_keyboard_event(event);
    } else if (window->flags & WindowWantsGlobalEvents) {
        window->receive_global_event(event);
    }
    return consumed;
}

bool UiLoop::deliver_event_to_children(UiWindow *window, SDL_Event *event, int offset_x, int offset_y) {
    for (std::vector<UiWindow *>::iterator iter = window->children.begin(); iter != window->children.end(); iter++) {
        bool consumed = deliver_event(*iter, event, offset_x, offset_y);
        if (consumed)
            return true;
    }
    return false;
}

void UiLoop::draw_window(UiWindow *window, const UiContext& context) {
    if (window->flags & WindowIsVisible) {
        window->draw(context);
        draw_children(window, context);
    }
}

void UiLoop::draw_children(UiWindow *window, const UiContext& context) {
    for (std::vector<UiWindow *>::iterator iter = window->children.begin(); iter != window->children.end(); iter++) {
        UiWindow *child = *iter;
        UiContext new_context = context.get_subcontext(child->x, child->y, child->width, child->height);
        draw_window(child, new_context);
    }
}

void UiLoop::update() {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
        deliver_event(root, &evt, 0, 0);
        if (evt.type == SDL_MOUSEBUTTONUP) {
            dragging_window = NULL;
        } else if (evt.type == SDL_MOUSEMOTION && dragging_window != NULL) {
            evt.type = drag_event_type;
            dragging_window->receive_mouse_event(&evt, 0, 0); // TODO pass in position of mouse relative to window
        }
    }

    if (root != NULL) {
        UiContext context(graphics, 0, 0);
        draw_window(root, context);
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
