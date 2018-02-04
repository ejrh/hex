#ifndef BUTTON_H
#define BUTTON_H

#include "hexav/graphics/graphics.h"
#include "hexav/ui/ui.h"


namespace hex {

class UiLabel;

class UiButton: public UiWindow {
public:
    UiButton(int x, int y, int width, int height, const std::string& label_text);
    UiButton(int x, int y, int width, int height, const std::string& label_text, Atom name);
    virtual ~UiButton();

    virtual bool receive_mouse_event(SDL_Event *evt, int x, int y);
    virtual bool receive_ui_event(SDL_Event *evt, UiWindow *control);
    virtual void draw(const UiContext& context);

public:
    UiLabel *label;
};

};

#endif
