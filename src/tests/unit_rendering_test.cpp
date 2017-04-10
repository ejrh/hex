#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/basics/hexgrid.h"
#include "hexutil/basics/noise.h"
#include "hexutil/messaging/builtin_messages.h"

#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"
#include "hexav/ui/ui.h"

#include "hexgame/game/game.h"

#include "hexview/resources/resource_loader.h"
#include "hexview/resources/resource_messages.h"
#include "hexview/view/unit_painter.h"
#include "hexview/view/unit_renderer.h"
#include "hexview/view/view.h"


void load_resources(Resources *resources, Graphics *graphics) {
    ImageLoader image_loader(resources, graphics);
    ResourceLoader loader(resources, &image_loader, NULL);
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
    TestWindow(UiLoop *loop, Graphics *graphics, Resources *resources, Game *game, UnitPainter *unit_painter, UnitRenderer *unit_renderer, UnitViewDef::pointer view_def):
            UiWindow(0, 0, 0, 0, WindowIsVisible|WindowIsActive|WindowWantsKeyboardEvents),
            loop(loop), graphics(graphics), resources(resources),
            game(game), unit_painter(unit_painter), unit_renderer(unit_renderer), view_def(view_def),
            last_update(0) {
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
            view_def = iter->second;
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_PAGEUP) {
            std::map<std::string,UnitViewDef::pointer>& map = resources->unit_view_defs;
            auto iter = map.find(view_def->name);
            if (iter == map.begin()) {
                iter = map.end();
            }
            iter--;
            view_def = iter->second;
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_r) {
            resources->unit_view_defs.clear();
            resources->tile_view_defs.clear();
            load_resources(resources, graphics);
            view_def = resources->unit_view_defs.get(view_def->name);
        }

        return false;
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
                Unit unit;
                unit_painter->repaint(unit_views[i], unit);

                TileViewDef::pointer tile_view_def = resources->tile_view_defs.get("grass");
                Image *ground = tile_view_def->animation.images[0].image;
                graphics->blit(ground, cx - ground->width / 2, cy - ground->height / 2, SDL_BLENDMODE_BLEND, 255);

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
    Resources *resources;
    Game *game;
    UnitPainter *unit_painter;
    UnitRenderer *unit_renderer;
    UnitViewDef::pointer view_def;
    UnitView unit_views[5];
    unsigned int last_update;
};

void run() {
    register_builtin_messages();
    register_resource_messages();
    register_property_names();
    register_builtin_interpreters();
    register_paint_interpreters();

    Graphics graphics;
    graphics.start("Unit rendering test", 800, 600, false);

    Game game;

    Resources resources;
    load_resources(&resources, &graphics);

    UnitPainter unit_painter(&game, NULL, &resources);
    UnitRenderer unit_renderer(&graphics, &resources);
    unit_renderer.generate_placeholders = false;

    UnitViewDef::pointer view_def = resources.unit_view_defs.begin()->second;

    UiLoop loop(&graphics, 25);
    TestWindow *test_window = new TestWindow(&loop, &graphics, &resources, &game, &unit_painter, &unit_renderer, view_def);
    loop.set_root_window(test_window);
    loop.run();

    graphics.stop();
}

int main(int argc, char *argv[]) {
    try {
        run();
    } catch (Error &ex) {
        BOOST_LOG_TRIVIAL(fatal) << "Failed with: " << ex.what();
    }

    return 0;
}
