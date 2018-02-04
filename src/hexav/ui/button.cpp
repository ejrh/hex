#include "common.h"

#include "hexav/graphics/graphics.h"
#include "hexav/graphics/font.h"
#include "hexav/ui/button.h"
#include "hexav/ui/controls.h"


namespace hex {

UiButton::UiButton(int x, int y, int width, int height, const std::string& label_text):
        UiWindow(x, y, width, height, WindowIsVisible|WindowIsActive|WindowWantsMouseEvents|WindowWantsUiEvents) {
    label = new UiLabel(2, 2, width - 4, height - 4);
    label->name = "label";
    label->set_format(TextFormat(SmallFont14, true, 255,255,255));
    label->set_text(label_text);
    add_child(label);
}

UiButton::UiButton(int x, int y, int width, int height, const std::string& label_text, Atom name):
        UiWindow(x, y, width, height, WindowIsVisible|WindowIsActive|WindowWantsMouseEvents|WindowWantsUiEvents, name) {
    label = new UiLabel(2, 2, width - 4, height - 4);
    label->name = "label";
    label->set_format(TextFormat(SmallFont14, true, 255,255,255));
    label->set_text(label_text);
    add_child(label);
}

UiButton::~UiButton() {
}

bool UiButton::receive_mouse_event(SDL_Event *evt, int x, int y) {
    if (evt->type == SDL_MOUSEBUTTONDOWN) {
        needs_repaint = true;
        return true;
    }
    if (evt->type == SDL_MOUSEBUTTONUP) {
        if (flags & WindowHasDragging) {
            push_ui_event(click_event_type, this, evt->button.button);
        }
        needs_repaint = true;
        return true;
    }
    return false;
}

bool UiButton::receive_ui_event(SDL_Event* evt, UiWindow *control) {
    if ((evt->type == focus_event_type || evt->type == unfocus_event_type) && control == this) {
        needs_repaint = true;
        return true;
    }
    return false;
}

void UiButton::draw(const UiContext& context) {
    context.fill_rectangle(200,150,150, 0, 0, width, height);
    if (flags & WindowHasFocus) {
        if (flags & WindowHasDragging) {
            context.fill_rectangle(50,50,50, 4, 4, width-8, height-8);
        } else {
            context.fill_rectangle(50,50,50, 2, 2, width-4, height-4);
        }
    } else {
        context.fill_rectangle(0,0,0, 2, 2, width-4, height-4);
    }
}

};
