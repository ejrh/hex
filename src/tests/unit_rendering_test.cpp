#include "common.h"

#include "hex/basics/error.h"
#include "hex/basics/hexgrid.h"
#include "hex/basics/noise.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"
#include "hex/game/game.h"
#include "hex/ui/ui.h"
#include "hex/view/unit_renderer.h"
#include "hex/view/view.h"

void load_resources(Resources *resources, Graphics *graphics) {
    ImageLoader image_loader(resources, graphics);
    ResourceLoader loader(resources, &image_loader);
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
    TestWindow(UiLoop *loop, Graphics *graphics, Resources *resources, Game *game, UnitRenderer *unit_renderer, UnitView *unit_view):
            UiWindow(0, 0, 0, 0),
            loop(loop), graphics(graphics), resources(resources),
            game(game), unit_renderer(unit_renderer), unit_view(unit_view),
            last_update(0) {
    }

    bool receive_event(SDL_Event *evt) {
        if (evt->type == SDL_QUIT
            || (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE)) {
            loop->running = false;
            return true;
        }

        if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_PAGEDOWN) {
            std::map<std::string,UnitViewDef::pointer>& map = resources->unit_view_defs;
            std::map<std::string,UnitViewDef::pointer>::iterator iter = map.find(unit_view->view_def->name);
            iter++;
            if (iter == map.end()) {
                iter = map.begin();
            }
            unit_view->view_def = iter->second;
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_PAGEUP) {
            std::map<std::string,UnitViewDef::pointer>& map = resources->unit_view_defs;
            std::map<std::string,UnitViewDef::pointer>::iterator iter = map.find(unit_view->view_def->name);
            if (iter == map.begin()) {
                iter = map.end();
            }
            iter--;
            unit_view->view_def = iter->second;
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_LEFT) {
            unit_view->facing--;
            if (unit_view->facing < 0)
                unit_view->facing = 5;
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_RIGHT) {
            unit_view->facing++;
            if (unit_view->facing > 5)
                unit_view->facing = 0;
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_SPACE) {
            ((int&) unit_view->posture)++;
            if (unit_view->posture > Dying)
                unit_view->posture = Holding;
        }

        return false;
    }

    void draw() {
        /* Update view */
        unsigned int ticks = SDL_GetTicks();
        unsigned int update_ms = ticks - last_update;
        last_update = ticks;

        unit_view->update(update_ms);

        /* Draw view */
        int cx = graphics->width / 2;
        int cy = graphics->height / 2;

        SDL_SetRenderDrawColor(graphics->renderer, 0,0,0, 255);
        SDL_RenderClear(graphics->renderer);

        TileViewDef::pointer tile_view_def = resources->tile_view_defs.get("grass");
        Image *ground = tile_view_def->animation.images[0].image;
        graphics->blit(ground, cx - ground->width / 2, cy - ground->height / 2, SDL_BLENDMODE_BLEND, 255);

        unit_renderer->draw_unit(cx, cy, *unit_view);
        //std::cerr << boost::format("w %d h %d cxo %d cyo %d cw %d ch %d\n") % image->width % image->height % image->clip_x_offset % image->clip_y_offset % image->clip_width % image->clip_height;

        TextFormat tf(graphics, SmallFont14, true, 250, 250, 250);
        tf.write_text(unit_view->view_def->name, cx, cy + 32);

        TextFormat tf2(graphics, SmallFont14, true, 150, 150, 150);
        std::ostringstream ss;
        ss << "Facing: " << unit_view->facing;
        tf2.write_text(ss.str(), cx, cy + 48);

        ss.str("");
        int bpm = unit_view->get_animation_def().bpm;
        ss << "Posture: " << posture_names[unit_view->posture] << " (" << bpm << " bpm)";
        tf2.write_text(ss.str(), cx, cy + 64);

        graphics->update();
    }

private:
    UiLoop *loop;
    Graphics *graphics;
    Resources *resources;
    Game *game;
    UnitRenderer *unit_renderer;
    UnitView *unit_view;
    unsigned int last_update;
};

void run() {
    Graphics graphics;
    graphics.start("Unit rendering test", 800, 600, false);

    Game game;

    Resources resources;
    load_resources(&resources, &graphics);

    UnitRenderer unit_renderer(&graphics, &resources);

    UnitView unit_view;
    unit_view.view_def = resources.get_unit_view_def("drow_archer");

    UiLoop loop(25);
    TestWindow test_window(&loop, &graphics, &resources, &game, &unit_renderer, &unit_view);
    loop.add_window(&test_window);
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
