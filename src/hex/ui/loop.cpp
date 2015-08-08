#include "common.h"

#include "hex/ui/ui.h"

Uint32 drag_event_type;


UiLoop::UiLoop(unsigned int frame_time): frame_time(frame_time), running(false) {
    drag_event_type = SDL_RegisterEvents(1);
}

void UiLoop::add_window(UiWindow *window) {
    windows.push_back(window);
}

UiWindow *UiLoop::find_window(int px, int py) {
    for (std::vector<UiWindow *>::reverse_iterator iter = windows.rbegin(); iter != windows.rend(); iter++) {
        if ((*iter)->contains(px, py)) {
            return *iter;
        }
    }
    return NULL;
}

void UiLoop::update() {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
        if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LMASK) {
            down_pos_x = evt.motion.x;
            down_pos_y = evt.motion.y;
        } else if (evt.type == SDL_MOUSEBUTTONUP) {
            if (dragging_window == NULL) {
                current_window = find_window(evt.button.x, evt.button.y);
                current_window->receive_event(&evt);
            } else {
                dragging_window = NULL;
            }
            continue;
        } else if (evt.type == SDL_MOUSEMOTION && dragging_window != NULL) {
            evt.type = drag_event_type;
            dragging_window->receive_event(&evt);
            continue;
        } else if (evt.type == SDL_MOUSEMOTION) {
            if (evt.motion.state == SDL_BUTTON_LMASK &&  abs(evt.motion.x - down_pos_x) > 4 && abs(evt.motion.y - down_pos_y) > 4) {
                current_window = find_window(evt.button.x, evt.button.y);
                dragging_window = current_window;
                evt.type = drag_event_type;
                dragging_window->receive_event(&evt);
                continue;
            }
        }

        for (std::vector<UiWindow *>::reverse_iterator iter = windows.rbegin(); iter != windows.rend(); iter++) {
            if ((*iter)->receive_event(&evt))
                break;
        }
    }

    for (std::vector<UiWindow *>::iterator iter = windows.begin(); iter != windows.end(); iter++) {
        (*iter)->draw();
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
