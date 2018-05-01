#include "common.h"

#include "hexutil/basics/point.h"

#include "hexav/ui/button.h"
#include "hexav/ui/controls.h"

#include "hexview/editor/brush.h"
#include "hexview/editor/editor.h"
#include "hexview/editor/palette.h"

#include "hexview/view/level_window.h"


namespace hex {

class BrushButton: public UiButton {
public:
    BrushButton(int x, int y, int width, int height, const std::string& label_text, std::unique_ptr<Brush> brush):
            UiButton(x, y, width, height, label_text), brush(std::move(brush)) {
    }

public:
    std::unique_ptr<Brush> brush;
};


class BrushSet: public UiWindow {
public:
    BrushSet(int x, int y, int width, int height, const std::string& name):
            UiWindow(x, y, width, height,
                     WindowIsActive|WindowIsVisible|WindowWantsKeyboardEvents|WindowWantsUiEvents, "brush_set") {
    }

    void add_brush(std::string name, std::unique_ptr<Brush> brush) {
        int ypos = children.empty() ? 10 : (children.rbegin()->get()->y + 22);
        BrushButton *button = new BrushButton(10, ypos, width-20, 20, name, std::move(brush));
        add_child(button);
    }

    bool receive_ui_event(SDL_Event *evt, UiWindow *control) {
        if (evt->type == click_event_type) {
            BrushButton *button = dynamic_cast<BrushButton *>(control);
            if (button) {
                set_brush(button);
            }
        }
        return false;
    }

    bool receive_keyboard_event(SDL_Event *evt) {
        if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_PAGEDOWN) {
            next_brush();
        }
        return false;
    }

    void draw(const UiContext& context) { }

private:
    void set_brush(BrushButton *new_button) {
        for (auto iter = children.begin(); iter != children.end(); iter++) {
            BrushButton *other_button = dynamic_cast<BrushButton *>(iter->get());
            if (other_button)
                other_button->clear_flag(WindowIsSelected);
        }

        PaletteWindow *palette = dynamic_cast<PaletteWindow *>(parent);
        if (palette)
            palette->set_brush(new_button->brush.get());

        new_button->set_flag(WindowIsSelected);
    }

    void next_brush() {
        BrushButton *first = nullptr;
        bool found = false;
        for (auto iter = children.begin(); iter != children.end(); iter++) {
            BrushButton *button = dynamic_cast<BrushButton *>(iter->get());
            if (button) {
                if (!first)
                    first = button;

                if (button->flags & WindowIsSelected) {
                    found = true;
                    continue;
                }

                if (found) {
                    first = button;
                    break;
                }
            }
        }

        set_brush(first);
    }
};


static const char *tab_names[] = { "Tile", "Feature" };

PaletteWindow::PaletteWindow(Game *game, GameView *view, LevelWindow *level_window, EditorWindow *editor_window):
        UiWindow(0, 0, palette_width, level_window->height,
             WindowWantsMouseEvents|WindowWantsKeyboardEvents|WindowWantsUiEvents, "palette"),
        game(game), level_window(level_window), editor_window(editor_window) {

    tab_bar = new UiTabBar(0, 0, width, 20);
    tab_bar->name = "tab_bar";
    add_child(tab_bar);
    int tab_width = tab_bar->width / NUM_TABS;
    for (int i = 0; i < NUM_TABS; i++) {
        std::ostringstream ss;
        ss << "tab_button" << i;
        tab_button[i] = new UiButton(tab_width * i, 0, tab_width, tab_bar->height, std::string(tab_names[i]));
        tab_button[i]->name = ss.str();
        tab_bar->add_child(tab_button[i]);

        std::ostringstream ss2;
        ss2 << "tab_panel" << i;

        tab_panel[i] = new BrushSet(tab_bar->x, tab_bar->height, tab_bar->width, height - tab_bar->height, std::string(tab_names[i]));
        tab_panel[i]->name = ss2.str();
        add_child(tab_panel[i]);
        tab_panel[i]->clear_flag(WindowIsActive|WindowIsVisible);
    }
    tab_bar->select_tab(tab_button[0]);
    tab_panel[0]->set_flag(WindowIsActive|WindowIsVisible);

    create_terrain_brushes();
}

void PaletteWindow::create_terrain_brushes() {
    BrushSet *brush_set = dynamic_cast<BrushSet *>(tab_panel[0]);

    for (auto iter = game->tile_types.begin(); iter != game->tile_types.end(); iter++) {
        std::unique_ptr<Brush> brush(new TileTypeBrush(iter->second));
        brush_set->add_brush(iter->first, std::move(brush));
    }

    brush_set = dynamic_cast<BrushSet *>(tab_panel[1]);

    for (auto iter = game->feature_types.begin(); iter != game->feature_types.end(); iter++) {
        std::unique_ptr<Brush> brush(new FeatureTypeBrush(iter->second));
        brush_set->add_brush(iter->first, std::move(brush));
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

void PaletteWindow::set_brush(Brush *brush) {
    editor_window->brush = brush;
}

bool PaletteWindow::receive_keyboard_event(SDL_Event *evt) {
    if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_1) {
        editor_window->paint_radius = 0;
    } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_2) {
        editor_window->paint_radius = 1;
    } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_3) {
        editor_window->paint_radius = 2;
    } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_4) {
        editor_window->paint_radius = 3;
    } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_5) {
        editor_window->paint_radius = 4;
    }
    return false;
}

bool PaletteWindow::receive_ui_event(SDL_Event *evt, UiWindow *control) {
    if (evt->type == tab_event_type && control == tab_bar) {
        for (int i = 0; i < NUM_TABS; i++) {
            if (tab_button[i]->flags & WindowIsSelected) {
                tab_panel[i]->set_flag(WindowIsActive | WindowIsVisible);
            } else {
                tab_panel[i]->clear_flag(WindowIsActive | WindowIsVisible);
            }
        }
    }
    return false;
}

void PaletteWindow::draw(const UiContext& context) {
    context.fill_rectangle(128, 128, 128, 0, 0, width, height);
}

};
