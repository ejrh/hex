#ifndef CONTROLS_H
#define CONTROLS_H

#include "hex/graphics/graphics.h"
#include "hex/graphics/font.h"
#include "hex/ui/ui.h"

class UiLabel: public UiWindow {
public:
    UiLabel(int x, int y, int width, int height);
    virtual ~UiLabel() { delete text_image; }

    virtual void draw(const UiContext& context);

    void set_text(const std::string& text);
    void set_format(const TextFormat& format);

public:
    std::string text;
    TextFormat format;
    Image *text_image;
};

class UiDialog: public UiWindow {
public:
    UiDialog(int x, int y, int width, int height, UiWindowFlags flags = WindowIsVisible|WindowIsActive);
    virtual ~UiDialog();

    virtual void draw(const UiContext& context);

    static const int border_thickeness = 4;
    static const int title_height = 20;

public:
    UiLabel *title;
};

class UiButton: public UiWindow {
public:
    UiButton(int x, int y, int width, int height, const std::string& label_text);
    virtual ~UiButton();

    virtual bool receive_mouse_event(SDL_Event *evt, int x, int y);
    virtual void draw(const UiContext& context);

public:
    UiLabel *label;
};

#endif
