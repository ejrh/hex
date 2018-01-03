#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/basics/hexgrid.h"
#include "hexutil/basics/noise.h"
#include "hexutil/messaging/builtin_messages.h"

#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"
#include "hexav/resources/resource_loader.h"
#include "hexav/resources/resource_messages.h"
#include "hexav/ui/ui.h"
#include "hexav/ui/window_painter.h"

#include "hexgame/game/game.h"

#include "hexview/view/unit_info_window.h"
#include "hexview/view/unit_painter.h"
#include "hexview/view/unit_renderer.h"
#include "hexview/view/view.h"
#include "hexview/view/view_resource_loader.h"
#include "hexview/view/view_resource_messages.h"


namespace hex {

void load_resources(ViewResources *resources, Graphics *graphics) {
    ImageLoader image_loader(resources, graphics);
    ViewResourceLoader loader(resources, &image_loader, NULL);
    loader.load(std::string("data/resources.txt"));
    resources->resolve_references();
}

bool reload_resources(ViewResources *resources, Graphics *graphics) {
    static std::time_t last_file_time = 0;

    try {
        boost::filesystem::path path = boost::filesystem::path("data/aow_view/unit_info_window.txt");
        std::time_t file_time = boost::filesystem::last_write_time(path);
        if (file_time != last_file_time) {
            ImageLoader image_loader(resources, graphics);
            ViewResourceLoader loader(resources, &image_loader, NULL);
            loader.load(std::string("data/aow_view/unit_info_window.txt"));
            resources->resolve_references();
            last_file_time = file_time;
            return true;
        }
    } catch (boost::filesystem::filesystem_error) {
        // ignore
    }

    return false;
}

class BackgroundWindow: public UiWindow {
public:
    BackgroundWindow():
            UiWindow(0, 0, 0, 0, WindowIsVisible|WindowIsActive|WindowWantsKeyboardEvents) {
    }

    void draw(const UiContext& context) {
        SDL_SetRenderDrawColor(context.graphics->renderer, 0,0,0, 255);
        SDL_RenderClear(context.graphics->renderer);
    }
};

class TestWindow: public UiWindow {
public:
    TestWindow(UiLoop *loop, Graphics *graphics, ViewResources *resources,
            UnitType::pointer initial_unit_type, UnitInfoWindow *unit_info_window):
            UiWindow(0, 0, 0, 0, WindowIsVisible|WindowIsActive|WindowWantsKeyboardEvents),
            loop(loop), graphics(graphics), resources(resources),
            unit_info_window(unit_info_window) {
        set_unit_type(initial_unit_type);
    }

    bool receive_keyboard_event(SDL_Event *evt) {
        if (evt->type == SDL_QUIT
            || (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE)) {
            loop->running = false;
            return true;
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_r) {
            load_resources(resources, graphics);
            unit_info_window->set_paint_script(resources->scripts, "UNIT_INFO_WINDOW");
        }

        return false;
    }

    void set_unit_type(UnitType::pointer new_unit_type) {
        unit_type = new_unit_type;
    }

    void draw(const UiContext& context) {
        if (reload_resources(resources, graphics)) {
            unit_info_window->set_paint_script(resources->scripts, "UNIT_INFO_WINDOW");
        }

        if (!(unit_info_window->flags & WindowIsVisible)) {
            loop->running = false;
        }

        graphics->update();
    }

private:
    UiLoop *loop;
    Graphics *graphics;
    ViewResources *resources;
    UnitType::pointer unit_type;
    UnitInfoWindow *unit_info_window;
};

void run() {
    register_builtin_messages();
    register_resource_messages();
    register_view_resource_messages();
    register_property_names();
    register_builtin_interpreters();
    register_paint_interpreters();
    register_window_interpreters();

    Graphics graphics("Unit info test", 480, 480, false);

    Game game;
    UnitType cat_type;
    cat_type.name = "cat";
    cat_type.properties[Walking] = 1;
    cat_type.properties[Moves] = 24;
    cat_type.properties[Sight] = 5;
    game.create_unit_type(cat_type);

    game.level.resize(3, 3);
    game.create_faction(1, "default", "default");

    game.create_unit_stack(1, Point(1, 1), 1);
    Unit::pointer cat = game.create_unit(1, "cat");

    ViewResources resources;
    load_resources(&resources, &graphics);

    UnitPainter unit_painter(&resources);
    UnitRenderer unit_renderer(&graphics, &resources);
    unit_renderer.generate_placeholders = false;

    if (game.unit_types.size() == 0)
        throw Error() << "No units to show";
    UnitType::pointer initial_unit_type = game.unit_types.begin()->second;

    int unit_info_window_x = (graphics.width - UnitInfoWindow::unit_info_window_width) / 2;
    int unit_info_window_y = (graphics.height - UnitInfoWindow::unit_info_window_height) / 2;

    WindowPainter window_painter(&resources);

    UiLoop loop(&graphics, 25, &window_painter);
    BackgroundWindow *background_window = new BackgroundWindow();
    UnitInfoWindow *unit_info_window = new UnitInfoWindow(unit_info_window_x, unit_info_window_y, UnitInfoWindow::unit_info_window_width, UnitInfoWindow::unit_info_window_height, &resources);
    unit_info_window->open(cat);
    background_window->add_child(unit_info_window);
    TestWindow *test_window = new TestWindow(&loop, &graphics, &resources, initial_unit_type, unit_info_window);
    background_window->add_child(test_window);
    loop.set_root_window(background_window);
    loop.run();
}

};


using namespace hex;

int main(int argc, char *argv[]) {
    try {
        run();
    } catch (Error &ex) {
        BOOST_LOG_TRIVIAL(fatal) << "Failed with: " << ex.what();
    }

    return 0;
}
