#include "common.h"

#include "hex/basics/error.h"
#include "hex/basics/noise.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"
#include "hex/game/game.h"
#include "hex/ui/ui.h"
#include <hex/resources/resources.h>

void load_resources(Resources *resources, Graphics *graphics) {
    ImageLoader image_loader(resources, graphics);
    ResourceLoader loader(resources, &image_loader);
    loader.load(std::string("data/resources.txt"));
    resources->resolve_references();
}

class TestWindow: public UiWindow {
public:
    TestWindow(UiLoop *loop, Graphics *graphics, Resources *resources):
            UiWindow(0, 0, 0, 0),
            loop(loop), graphics(graphics), resources(resources)
    {
        current = resources->images.begin()->first;
    }

    bool receive_event(SDL_Event *evt) {
        if (evt->type == SDL_QUIT
            || (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE)) {
            loop->running = false;
            return true;
        }

        if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_PAGEDOWN) {
            ImageMap::iterator iter = resources->images.find(current);
            iter++;
            if (iter == resources->images.end()) {
                iter = resources->images.begin();
            }
            current = iter->first;
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_PAGEUP) {
            ImageMap::iterator iter = resources->images.find(current);
            if (iter == resources->images.begin()) {
                iter = resources->images.end();
            }
            iter--;
            current = iter->first;
        }

        return false;
    }

    void draw() {
        int cx = graphics->width / 2;
        int cy = graphics->height / 2;
        Image *image = resources->images[current];

        SDL_SetRenderDrawColor(graphics->renderer, 0,0,0, 255);
        SDL_RenderClear(graphics->renderer);

        int x = cx - image->width/2;
        int y = cy - image->height/2;
        int w = image->width;
        int h = image->height;

        graphics->blit(image, x, y, SDL_BLENDMODE_BLEND);

        graphics->draw_rectangle(100, 100, 100, x - 2, y - 2, w + 4, h + 4);

        graphics->draw_rectangle(100, 100, 100, x + image->clip_x_offset, y + image->clip_y_offset, image->clip_width, image->clip_height);

        TextFormat tf(graphics, SmallFont14, true, 250, 250, 250);
        tf.write_text(current, cx, y + h + 16);

        TextFormat tf2(graphics, SmallFont14, true, 150, 150, 150);
        std::ostringstream ss;
        ss << boost::format("W %d, H %d") % image->width % image->height;
        tf2.write_text(ss.str(), cx, y + h + 32);

        graphics->update();
    }

private:
    UiLoop *loop;
    Graphics *graphics;
    Resources *resources;
    std::string current;
};

void run() {
    Graphics graphics;
    graphics.start("Image test", 800, 600, false);

    Resources resources;
    load_resources(&resources, &graphics);

    UiLoop loop(25);
    TestWindow test_window(&loop, &graphics, &resources);
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
