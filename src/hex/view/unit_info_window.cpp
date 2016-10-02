#include "common.h"

#include "hex/game/game.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"
#include "hex/view/unit_info_window.h"
#include "hex/view/view.h"


static std::map<PropertyName, PropertyValue> get_all_properties(const Unit& unit) {
    std::map<PropertyName, PropertyValue> properties;
    for (std::map<PropertyName, PropertyValue>::const_iterator iter = unit.properties.data.begin(); iter != unit.properties.data.end(); iter++) {
        properties[iter->first] = iter->second;
    }
    for (std::map<PropertyName, PropertyValue>::const_iterator iter = unit.type->properties.data.begin(); iter != unit.type->properties.data.end(); iter++) {
        if (properties.find(iter->first) == properties.end())
            properties[iter->first] = iter->second;
    }
    return properties;
}

UnitInfoWindow::UnitInfoWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, GameView *view):
        UiWindow(x, y, width, height), resources(resources), graphics(graphics), view(view), is_open(false) {
}

bool UnitInfoWindow::contains(int px, int py) {
    if (!is_open)
        return false;
    else
        return UiWindow::contains(px, py);
}


bool UnitInfoWindow::receive_event(SDL_Event *evt) {
    if (!is_open)
        return false;

    if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE) {
        close();
        return true;
    }

    return false;
}

void UnitInfoWindow::draw() {
    if (!is_open)
        return;

    graphics->fill_rectangle(200,150,150, x, y, width, height);
    graphics->fill_rectangle(0,0,0, x+4, y+4, width-8, height-8);

    TextFormat tf(SmallFont14, true, 255,255,255);
    const std::string& title = current_unit->type->name;
    tf.write_text(graphics, title, x + width/2, y + 12);

    TextFormat tf2(SmallFont10, false, 192,192,192);
    int x_offset = x + 12;
    int y_offset = y + 20;
    std::map<PropertyName, PropertyValue> properties = get_all_properties(*current_unit);
    for (std::map<PropertyName, PropertyValue>::const_iterator iter = properties.begin(); iter != properties.end(); iter++) {
        std::ostringstream ss;
        ss << get_property_name_str(iter->first);
        ss << ": ";
        ss << iter->second.value;
        tf2.write_text(graphics, ss.str(), x_offset, y_offset);
        y_offset += 12;
    }
}

void UnitInfoWindow::open(Unit::pointer current_unit) {
    this->current_unit = current_unit;
    is_open = true;
}

void UnitInfoWindow::close() {
    is_open = false;
}
