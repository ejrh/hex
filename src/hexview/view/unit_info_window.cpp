#include "common.h"

#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"

#include "hexgame/game/game.h"

#include "hexview/view/unit_info_window.h"
#include "hexview/view/view.h"


static std::unordered_map<Atom, Datum> get_all_properties(const Unit& unit) {
    std::unordered_map<Atom, Datum> properties;
    for (auto iter = unit.properties.data.begin(); iter != unit.properties.data.end(); iter++) {
        properties[iter->first] = iter->second;
    }
    for (auto iter = unit.type->properties.data.begin(); iter != unit.type->properties.data.end(); iter++) {
        if (properties.find(iter->first) == properties.end())
            properties[iter->first] = iter->second;
    }
    return properties;
}

UnitInfoWindow::UnitInfoWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, GameView *view):
        UiWindow(x, y, width, height, WindowWantsKeyboardEvents|WindowWantsUiEvents, "unit_info"),
        resources(resources), graphics(graphics), view(view) {
    set_paint_script(resources->scripts, "UNIT_INFO_WINDOW");

    close_button = new UiButton(151, 380, 119, 27, "Close");
    close_button->name = "close_button";
    add_child(close_button);

    tab_bar = new UiTabBar(21, 163, 376, 22);
    tab_bar->name = "tab_bar";
    add_child(tab_bar);
    int tab_width = tab_bar->width / 3;
    for (int i = 0; i < 3; i++) {
        std::ostringstream ss;
        ss << "tab_button" << i;
        tab_button[i] = new UiButton(tab_width * i, 0, tab_width, tab_bar->height, ss.str());
        tab_button[i]->name = ss.str();
        tab_bar->add_child(tab_button[i]);

        std::ostringstream ss2;
        ss2 << "tab_panel" << i;

        tab_panel[i] = new UiWindow(tab_bar->x, tab_bar->y + tab_bar->height, tab_bar->width, 195, WindowWantsMouseEvents|WindowWantsUiEvents);
        tab_panel[i]->name = ss2.str();
        add_child(tab_panel[i]);
    }
    tab_bar->select_tab(tab_button[0]);
    tab_panel[0]->set_flag(WindowIsActive|WindowIsVisible);

    title = new UiLabel(10, 6, width - 20, 12);
    title->set_format(TextFormat(SmallFont14, true, 255,255,255));
    add_child(title);

    /* Set up tab panel 0 */
    abilities_list = new UiTextList(10, 10, tab_panel[0]->width - 20, tab_panel[0]->height - 20);
    tab_panel[0]->add_child(abilities_list);
}

bool UnitInfoWindow::receive_keyboard_event(SDL_Event *evt) {
    if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE) {
        close();
        return true;
    }

    return false;
}

bool UnitInfoWindow::receive_ui_event(SDL_Event *evt, UiWindow *control) {
    if (evt->type == click_event_type && control == close_button) {
        close();
        return true;
    } else if (evt->type == tab_event_type && control == tab_bar) {
        for (int i = 0; i < 3; i++) {
            if (tab_button[i]->flags & WindowIsSelected) {
                tab_panel[i]->set_flag(WindowIsActive | WindowIsVisible);
            } else {
                tab_panel[i]->clear_flag(WindowIsActive | WindowIsVisible);
            }
        }
    }

    return false;
}

void UnitInfoWindow::draw(const UiContext& context) {
    UiWindow::draw(context);

    if (!current_unit)
        return;
}

void UnitInfoWindow::open(Unit::pointer current_unit) {
    this->current_unit = current_unit;
    title->set_text(current_unit->type->name);
    set_flag(WindowIsVisible|WindowIsActive);

    TextFormat tf2(SmallFont10, false, 192,192,192);
    int x_offset = x + 12;
    int y_offset = y + /* title_height + */ 12;

    // Populate abilities list
    abilities_list->clear();
    std::unordered_map<Atom, Datum> properties = get_all_properties(*current_unit);
    for (auto iter = properties.begin(); iter != properties.end(); iter++) {
        std::ostringstream ss;
        ss << iter->first;
        ss << ": ";
        ss << iter->second.value;
        abilities_list->add_line(ss.str());
    }

    needs_repaint = true;
}

void UnitInfoWindow::close() {
    current_unit.reset();
    clear_flag(WindowIsVisible|WindowIsActive);
}
