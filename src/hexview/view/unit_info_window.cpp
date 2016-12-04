#include "common.h"

#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"

#include "hexgame/game/game.h"

#include "hexview/view/unit_info_window.h"
#include "hexview/view/view.h"


static std::map<Atom, Datum> get_all_properties(const Unit& unit) {
    std::map<Atom, Datum> properties;
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
        UiDialog(x, y, width, height, WindowWantsKeyboardEvents),
        resources(resources), graphics(graphics), view(view) {
}

bool UnitInfoWindow::receive_keyboard_event(SDL_Event *evt) {
    if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE) {
        close();
        return true;
    }

    return false;
}

void UnitInfoWindow::draw(const UiContext& context) {
    UiDialog::draw(context);

    if (!current_unit)
        return;

    TextFormat tf2(SmallFont10, false, 192,192,192);
    int x_offset = x + 12;
    int y_offset = y + title_height + 12;
    std::map<Atom, Datum> properties = get_all_properties(*current_unit);
    for (auto iter = properties.begin(); iter != properties.end(); iter++) {
        std::ostringstream ss;
        ss << iter->first;
        ss << ": ";
        ss << iter->second.value;
        tf2.write_text(graphics, ss.str(), x_offset, y_offset);
        y_offset += 12;
    }
}

void UnitInfoWindow::open(Unit::pointer current_unit) {
    this->current_unit = current_unit;
    title->set_text(current_unit->type->name);
    set_flag(WindowIsVisible|WindowIsActive);
}

void UnitInfoWindow::close() {
    current_unit.reset();
    clear_flag(WindowIsVisible|WindowIsActive);
}
