#include "common.h"

#include "hexutil/basics/error.h"

#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"
#include "hexav/ui/ui.h"
#include "hexav/ui/controls.h"

class TestWindow: public UiWindow {
public:
    TestWindow(UiLoop *loop, Graphics *graphics):
            UiWindow(0, 0, 0, 0, WindowIsVisible|WindowIsActive|WindowWantsKeyboardEvents),
            loop(loop), graphics(graphics) {
    }

    bool receive_keyboard_event(SDL_Event *evt) {
        if (evt->type == SDL_QUIT
            || (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE)) {
            loop->running = false;
            return true;
        }

        return false;
    }

    void draw(const UiContext& context) {
        SDL_SetRenderDrawColor(graphics->renderer, 0,0,0, 255);
        SDL_RenderClear(graphics->renderer);
    }

private:
    UiLoop *loop;
    Graphics *graphics;
};

class TopWindow: public UiWindow {
public:
    TopWindow(Graphics *graphics):
            UiWindow(0, 0, 0, 0, WindowIsVisible), graphics(graphics) {
    }

    void draw(const UiContext& context) {
        graphics->update();
    }

public:
    Graphics *graphics;
};

class TestCloseButton: public UiButton {
public:
    TestCloseButton(int x, int y, int width, int height):
            UiButton(x, y, width, height, "Close") { }

    bool receive_mouse_event(SDL_Event *evt, int x, int y) {
        if (evt->type == SDL_MOUSEBUTTONDOWN) {
            parent->clear_flag(WindowIsVisible);
            return true;
        }
        return false;
    }
};

class TestDialog: public UiDialog {
public:
    TestDialog(int x, int y, int width, int height, Graphics *graphics):
            UiDialog(x, y, width, height, WindowIsVisible|WindowIsActive|WindowWantsMouseEvents|WindowWantsKeyboardEvents) {
        title->set_text("Test Dialog");
        close_button = new TestCloseButton(width/2 - 50, height - 30, 100, 20);
        add_child(close_button);
        text_list = new UiTextList(10, 40, width-20, height-100);
        add_child(text_list);
    }

    bool receive_mouse_event(SDL_Event *evt, int x, int y) {
        std::ostringstream ss;
        ss << evt->common.timestamp;
        ss << " Mouse event: type " << evt->type << " pos " << x << "," << y;
        text_list->add_line(ss.str());
        if (evt->type == SDL_MOUSEBUTTONDOWN) {
            return true;
        }
        else if (evt->type == drag_event_type) {
            return true;
        }
        return false;
    }

    bool receive_keyboard_event(SDL_Event *evt) {
        std::ostringstream ss;
        ss << evt->common.timestamp;
        ss << " Keyboard event: " << (char) evt->key.keysym.sym;
        text_list->add_line(ss.str());
        return false;
    }

private:
    UiButton *close_button;
    UiTextList *text_list;
};

void run() {
    Graphics graphics("UI test", 800, 600, false);

    UiLoop loop(&graphics, 25);
    TestWindow *test_window = new TestWindow(&loop, &graphics);
    TestDialog *test_dialog = new TestDialog(200, 100, 400, 400, &graphics);
    TopWindow *top_window = new TopWindow(&graphics);
    test_window->add_child(test_dialog);
    test_window->add_child(top_window);
    loop.set_root_window(test_window);
    loop.run();
}

int main(int argc, char *argv[]) {
    try {
        run();
    } catch (Error &ex) {
        BOOST_LOG_TRIVIAL(fatal) << "Failed with: " << ex.what();
    }

    return 0;
}
