#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/basics/hexgrid.h"
#include "hexutil/basics/noise.h"
#include "hexutil/messaging/builtin_messages.h"

#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"
#include "hexav/resources/resource_messages.h"
#include "hexav/ui/ui.h"

#include "hexgame/game/game.h"

#include "hexview/view/tile_painter.h"
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

static std::string posture_names[] = {
    "Holding",
    "Moving",
    "Attacking",
    "Recoiling",
    "Dying"
};


class TestWindow: public UiWindow {
public:
    TestWindow(UiLoop *loop, Graphics *graphics, ViewResources *resources, Game *game,
            TilePainter *tile_painter, UnitPainter *unit_painter, UnitRenderer *unit_renderer, UnitViewDef::pointer initial_view_def):
            UiWindow(0, 0, 0, 0, WindowIsVisible|WindowIsActive|WindowWantsKeyboardEvents),
            loop(loop), graphics(graphics), resources(resources), game(game),
            tile_painter(tile_painter),
            unit_painter(unit_painter), unit_renderer(unit_renderer),
            last_update(0) {
        set_view_def(initial_view_def);
    }

    bool receive_keyboard_event(SDL_Event *evt) {
        if (evt->type == SDL_QUIT
            || (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE)) {
            loop->running = false;
            return true;
        }

        if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_PAGEDOWN) {
            std::map<std::string,UnitViewDef::pointer>& map = resources->unit_view_defs;
            auto iter = map.find(view_def->name);
            iter++;
            if (iter == map.end()) {
                iter = map.begin();
            }
            set_view_def(iter->second);
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_PAGEUP) {
            std::map<std::string,UnitViewDef::pointer>& map = resources->unit_view_defs;
            auto iter = map.find(view_def->name);
            if (iter == map.begin()) {
                iter = map.end();
            }
            iter--;
            set_view_def(iter->second);
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_r) {
            resources->unit_view_defs.clear();
            resources->tile_view_defs.clear();
            load_resources(resources, graphics);
            view_def = resources->unit_view_defs.get(view_def->name);
        }

        return false;
    }

    void set_view_def(UnitViewDef::pointer new_view_def) {
        view_def = new_view_def;
        std::string background_name;
        boost::regex pattern_re(".*_ship|gall.*|sea_.*|mermaid");
        if (boost::regex_match(view_def->name, pattern_re)) {
            background_name = "water";
        } else {
            background_name = "grass";
        }

        tile_view.view_def = resources->get_tile_view_def(background_name);
        tile.type = game->tile_types.get(background_name);

        tile_painter->repaint_tile(tile_view, tile, Point(0, 0));
    }

    void draw(const UiContext& context) {
        /* Update view */
        unsigned int ticks = SDL_GetTicks();
        unsigned int update_ms = ticks - last_update;
        last_update = ticks;

        for (int i = 0; i < 5; i++) {
            unit_views[i].view_def = view_def;
            unit_views[i].posture = (UnitPosture) i;
            unit_views[i].update(update_ms);
        }

        /* Draw view */
        SDL_SetRenderDrawColor(graphics->renderer, 0,0,0, 255);
        SDL_RenderClear(graphics->renderer);

        for (int i = 0; i < 5; i++) {
            int cy = (int) (((graphics->height - 64) / 5) * (i + 0.5) + 16);

            for (int j = 0; j < 6; j++) {
                int cx = (int) ((graphics->width / 6) * (j + 0.5));
                unit_views[i].facing = j;
                unit_views[i].variation = j;
                Unit unit;
                unit_painter->repaint(unit_views[i], unit);

                tile_view.tile_paint.render(cx, cy, 0, graphics);

                unit_renderer->draw_unit(cx, cy, unit_views[i]);
            }

            unit_views[i].posture = (UnitPosture) i;

            TextFormat tf2(SmallFont14, true, 150, 150, 150);
            std::ostringstream ss;
            ss << posture_names[unit_views[i].posture] << " (" << "???" << " bpm, duration " << "???" << "ms)";
            tf2.write_text(graphics, ss.str(), graphics->width / 2, cy + 32);
        }

        int cx = graphics->width / 2;
        int cy = graphics->height - 64;

        TextFormat tf(SmallFont14, true, 250, 250, 250);
        tf.write_text(graphics, view_def->name, cx, cy + 32);

        graphics->update();
    }

private:
    UiLoop *loop;
    Graphics *graphics;
    ViewResources *resources;
    Game *game;
    TilePainter *tile_painter;
    UnitPainter *unit_painter;
    UnitRenderer *unit_renderer;
    UnitViewDef::pointer view_def;
    Tile tile;
    TileView tile_view;
    UnitView unit_views[5];
    unsigned int last_update;
};

void run() {
    register_builtin_messages();
    register_resource_messages();
    register_view_resource_messages();
    register_property_names();
    register_builtin_interpreters();
    register_paint_interpreters();

    Graphics graphics("Unit rendering test", 800, 600, false);

    Game game;
    game.create_tile_type(TileType("grass"));
    game.create_tile_type(TileType("water"));

    ViewResources resources;
    load_resources(&resources, &graphics);

    TilePainter tile_painter(&game, NULL, &resources);

    UnitPainter unit_painter(&resources);
    UnitRenderer unit_renderer(&graphics, &resources);
    unit_renderer.generate_placeholders = false;

    if (resources.unit_view_defs.size() == 0)
        throw Error() << "No unit views to show";
    UnitViewDef::pointer initial_view_def = resources.unit_view_defs.begin()->second;

    UiLoop loop(&graphics, 25);
    TestWindow *test_window = new TestWindow(&loop, &graphics, &resources, &game, &tile_painter, &unit_painter, &unit_renderer, initial_view_def);
    loop.set_root_window(test_window);
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
