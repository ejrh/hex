#include "common.h"

#include "hexutil/basics/point.h"

#include "hexav/ui/controls.h"

#include "hexview/editor/brush.h"
#include "hexview/editor/editor.h"
#include "hexview/editor/palette.h"

#include "hexview/view/level_window.h"


class BrushButton: public UiButton {
public:
    BrushButton(int x, int y, int width, int height, const std::string& label_text, Brush *brush):
            UiButton(x, y, width, height, label_text), brush(brush) {
    }

    bool receive_mouse_event(SDL_Event *evt, int x, int y) {
        if (evt->type == SDL_MOUSEBUTTONUP && evt->button.button == SDL_BUTTON_LEFT) {
            brush->set_type(label->text);
            return true;
        }
        return false;
    }

private:
    Brush *brush;
};


PaletteWindow::PaletteWindow(Game *game, GameView *view, LevelWindow *level_window, EditorWindow *editor_window):
        UiWindow(0, 0, palette_width, level_window->height,
             WindowWantsMouseEvents|WindowWantsKeyboardEvents),
        game(game), view(view), level_window(level_window), editor_window(editor_window), brush(new Brush(game, view)) {
    editor_window->brush = brush;

    create_brush_buttons();
}

void PaletteWindow::create_brush_buttons() {
    int ypos = 10;

    for (auto iter = game->tile_types.begin(); iter != game->tile_types.end(); iter++) {
        BrushButton *button = new BrushButton(10, ypos, width-20, 20, iter->first, brush);
        add_child(button);

        ypos += 22;
    }
}

void PaletteWindow::show() {
    if (flags & WindowIsVisible)
        return;

    level_window->x += palette_width;
    level_window->width -= palette_width;
    level_window->shift(0, 0);
    editor_window->x += palette_width;
    editor_window->width -= palette_width;
    editor_window->set_flag(WindowIsActive);
    set_flag(WindowIsActive|WindowIsVisible);
}

void PaletteWindow::hide() {
    if (!(flags & WindowIsVisible))
        return;

    level_window->x -= palette_width;
    level_window->width += palette_width;
    level_window->shift(0, 0);
    editor_window->x -= palette_width;
    editor_window->width += palette_width;
    editor_window->clear_flag(WindowIsActive);
    clear_flag(WindowIsActive|WindowIsVisible);
}

void PaletteWindow::toggle() {
    if (flags & WindowIsVisible)
        hide();
    else
        show();
}

bool PaletteWindow::receive_mouse_event(SDL_Event *evt, int x, int y) {
    return false;
}

bool PaletteWindow::receive_keyboard_event(SDL_Event *evt) {
    if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_1) {
        brush->paint_radius = 0;
    } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_2) {
        brush->paint_radius = 1;
    } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_3) {
        brush->paint_radius = 2;
    } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_4) {
        brush->paint_radius = 3;
    } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_5) {
        brush->paint_radius = 4;
    } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_PAGEDOWN) {
        brush->next_type();
        return true;
    }
    return false;
}

void PaletteWindow::draw(const UiContext& context) {
    context.fill_rectangle(128, 128, 128, x, y, width, height);
}
