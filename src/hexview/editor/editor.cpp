#include "common.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"

#include "hexview/editor/brush.h"
#include "hexview/editor/editor.h"
#include "hexview/view/level_window.h"
#include "hexview/view/view.h"


EditorWindow::EditorWindow(GameView *view, LevelWindow *level_window):
    UiWindow(level_window->x, level_window->y, level_window->width, level_window->height,
             WindowWantsMouseEvents),
    view(view), level_window(level_window), brush(nullptr) {
}

bool EditorWindow::receive_mouse_event(SDL_Event *evt, int x, int y) {
    if (evt->type == drag_event_type) {
        brush->paint_tiles(view->level_view.highlight_tile);
        return true;
    } else if (evt->type == SDL_MOUSEBUTTONDOWN && evt->button.button == SDL_BUTTON_LEFT) {
        return true;
    } else if (evt->type == SDL_MOUSEBUTTONUP && evt->button.button == SDL_BUTTON_LEFT) {
        brush->paint_tiles(view->level_view.highlight_tile);
        return true;
    }
    return false;
}
