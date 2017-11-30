#include "common.h"

#include "hexav/graphics/graphics.h"
#include "hexav/graphics/font.h"
#include "hexav/ui/controls.h"


UiLabel::UiLabel(int x, int y, int width, int height):
        UiWindow(x, y, width, height, WindowIsVisible), text_image(NULL) {
}

UiLabel::~UiLabel() {
    delete text_image;
}

void UiLabel::draw(const UiContext& context) {
    if (this->text_image == NULL && text.size() > 0) {
        this->text_image = format.write_to_image(context.graphics, text);
    }

    if (this->text_image != NULL) {
        if (format.centered) {
            context.blit(text_image, width/2, height/2);
        } else {
            context.blit(text_image, 0, 0);
        }
    }
}

void UiLabel::set_text(const std::string& text) {
    this->text = text;
    delete text_image;
    text_image = NULL;
}

void UiLabel::set_format(const TextFormat& format) {
    this->format = format;
    delete text_image;
    text_image = NULL;
}


UiDialog::UiDialog(int x, int y, int width, int height, UiWindowFlags flags):
        UiWindow(x, y, width, height, flags) {
    title = new UiLabel(border_thickeness, border_thickeness, width - border_thickeness*2, title_height);
    title->set_format(TextFormat(SmallFont14, true, 255,255,255));
    add_child(title);
}

UiDialog::~UiDialog() {
}


void UiDialog::draw(const UiContext& context) {
    context.fill_rectangle(200,150,150, 0, 0, width, height);
    context.fill_rectangle(0,0,0, border_thickeness, border_thickeness, width - border_thickeness*2, title_height);
    context.fill_rectangle(0,0,0, border_thickeness, title_height + border_thickeness*2, width - border_thickeness*2, height - title_height - border_thickeness*3);
}


UiButton::UiButton(int x, int y, int width, int height, const std::string& label_text):
        UiWindow(x, y, width, height, WindowIsVisible|WindowIsActive|WindowWantsMouseEvents|WindowWantsUiEvents) {
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
        push_ui_event(click_event_type, this);
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
        context.fill_rectangle(50,50,50, 2, 2, width-4, height-4);
    } else {
        context.fill_rectangle(0,0,0, 2, 2, width-4, height-4);
    }
}


UiTextList::UiTextList(int x, int y, int width, int height):
        UiWindow(x, y, width, height, WindowIsVisible) {
}

UiTextList::~UiTextList() {
}

void UiTextList::draw(const UiContext& context) {
    context.draw_rectangle(200,150,150, 0, 0, width, height);
}

void UiTextList::add_line(const std::string& text) {
    int next_label_y = 5 + 16 * children.size();
    if (next_label_y > height-16) {
        children.erase(children.begin());
        for (int i = 0; i < children.size(); i++)
            children[i]->y -= 16;
        next_label_y -= 16;
    }
    UiLabel *new_label = new UiLabel(5, next_label_y, width-10, 16);
    new_label->set_text(text);
    add_child(new_label);
}

void UiTextList::clear() {
    children.clear();
}


UiTabBar::UiTabBar(int x, int y, int width, int height):
        UiWindow(x, y, width, height, WindowIsVisible|WindowIsActive|WindowWantsUiEvents),
        current_tab(nullptr) {
}

UiTabBar::~UiTabBar() {
}

bool UiTabBar::receive_ui_event(SDL_Event* evt, UiWindow *control) {
    if (evt->type == click_event_type) {
        for (auto iter = children.begin(); iter != children.end(); iter++) {
            if (control == iter->get()) {
                select_tab(control);
                return true;
            }
        }
    }
    return false;
}

void UiTabBar::draw(const UiContext& context) {
    context.draw_rectangle(200,150,150, 0, 0, width, height);
}

void UiTabBar::select_tab(UiWindow *tab) {
    if (current_tab != tab) {
        if (current_tab) {
            current_tab->clear_flag(WindowIsSelected);
            current_tab->needs_repaint = true;
        }
        current_tab = tab;
        if (current_tab) {
            current_tab->set_flag(WindowIsSelected);
            current_tab->needs_repaint = true;
        }
    }
    push_ui_event(tab_event_type, this);
}
