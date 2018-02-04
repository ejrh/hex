#ifndef CONTROLS_H
#define CONTROLS_H

#include "hexav/graphics/graphics.h"
#include "hexav/graphics/font.h"
#include "hexav/ui/ui.h"


namespace hex {

class UiLabel: public UiWindow {
public:
    UiLabel(int x, int y, int width, int height);
    virtual ~UiLabel();

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

class UiTextList: public UiWindow {
public:
    UiTextList(int x, int y, int width, int height);
    virtual ~UiTextList();

    virtual void draw(const UiContext& context);

    void add_line(const std::string& text);
    void clear();
};

class UiTabBar: public UiWindow {
public:
    UiTabBar(int x, int y, int width, int height);
    virtual ~UiTabBar();

    virtual bool receive_ui_event(SDL_Event *evt, UiWindow *control);
    virtual void draw(const UiContext& context);

    void select_tab(UiWindow *tab);

public:
    UiWindow *current_tab;
};

};

#endif
