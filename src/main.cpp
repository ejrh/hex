#include "common.h"

#include "hex/basics/error.h"
#include "hex/noise.h"
#include "hex/graphics/graphics.h"
#include "hex/messaging/serialiser.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"
#include "hex/messaging/updater.h"
#include "hex/messaging/dispatcher.h"
#include "hex/messaging/event_pusher.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_serialisation.h"
#include "hex/game/game_updater.h"
#include "hex/game/game_arbiter.h"
#include "hex/networking/networking.h"
#include "hex/view/view.h"
#include "hex/view/view_updater.h"
#include "hex/view/level_renderer.h"


void generate_level(Level &level, TileType *water, TileType *desert, TileType *grass) {
    PerlinNoise noise(5, 5);
    PerlinNoise noise2(10, 10);

    for (int i = 0; i < level.height; i++) {
        for (int j = 0; j < level.width; j++) {
            float px = (float) j / level.width;
            float py = (float) i / level.height;
            if (j % 2 == 1)
                py += 0.5f / level.height;

            float value = (noise.value(px, py) + noise2.value(py, px))/6.0f;
            if (value < 0.0f)
                level.tiles[i][j].type = water;
            else if (value < 0.5f)
                level.tiles[i][j].type = desert;
            else
                level.tiles[i][j].type = grass;
        }
    }
}

void create_game(Game& game, Updater& updater) {
    int width = 30;
    int height = 20;

    replay_messages("data/tile_types.txt", updater);
    replay_messages("data/unit_types.txt", updater);

    boost::shared_ptr<Level> level = boost::make_shared<Level>(width, height);
    generate_level(*level, game.tile_types["water"], game.tile_types["desert"], game.tile_types["grass"]);

    updater.receive(boost::make_shared<SetLevelMessage>(level));

    for (int i = 0; i < 10; i++) {
        int tx = rand() % level->width;
        int ty = rand() % level->height;
        updater.receive(boost::make_shared<CreateStackMessage>(i, Point(tx, ty)));
        updater.receive(boost::make_shared<CreateUnitMessage>(i, "dragon"));
    };

    updater.flush();
}

extern void connect(std::string server, int port);

void run() {
    Graphics graphics;

    graphics.start();

    Resources resources;

    load_resources("data/images.txt", resources, &graphics);
    load_resources("data/ui.txt", resources, &graphics);
    load_resources("data/tile_views.txt", resources, &graphics);
    load_resources("data/unit_views.txt", resources, &graphics);

    resources.resolve_references();

    EventPusher event_pusher;
    Server server(9999, &event_pusher);

    Game game;
    Updater updater(1000);
    GameArbiter arbiter(&game, &updater);
    Dispatcher dispatcher(&arbiter);

    GameUpdater game_updater(&game);
    updater.subscribe(&game_updater);

    GameView game_view(graphics.width, graphics.height, &resources, &dispatcher);
    ViewUpdater view_updater(&game, &game_view, &resources);
    updater.subscribe(&view_updater);

    create_game(game, updater);

    LevelRenderer level_renderer(&graphics, &resources, game.level, &game_view.level_view);

    server.start();

    int down_pos_x = 0, down_pos_y = 0;
    bool dragging = false;

    SDL_Event evt;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT
                || (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE))
                running = false;

            if (evt.type == SDL_MOUSEMOTION) {
                game_view.level_view.set_mouse_position(evt.motion.x, evt.motion.y);
            }

            if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LMASK) {
                down_pos_x = evt.motion.x;
                down_pos_y = evt.motion.y;
            } else if (evt.type == SDL_MOUSEBUTTONUP && evt.button.button == SDL_BUTTON_LMASK) {
                if (!dragging) {
                    game_view.level_view.left_click(evt.button.x, evt.button.y);
                }
                dragging = false;
            } else if (evt.type == SDL_MOUSEBUTTONUP && evt.button.button == 3) {
                if (!dragging) {
                    game_view.level_view.right_click(evt.button.x, evt.button.y);
                }
                dragging = false;
            } else if (evt.type == SDL_MOUSEMOTION && dragging) {
                game_view.level_view.shift(evt.motion.xrel, evt.motion.yrel);
            } else if (evt.type == SDL_MOUSEMOTION) {
                if (evt.motion.state == SDL_BUTTON_LMASK &&  abs(evt.motion.x - down_pos_x) > 4 && abs(evt.motion.y - down_pos_y) > 4)
                    dragging = true;
            }

            if (evt.type == event_pusher.event_type) {
                boost::shared_ptr<Message> msg = event_pusher.get_message(evt);
                std::ostringstream ss;
                Serialiser writer(ss);
                writer << msg.get();
                trace("Recieved from network: %s", ss.str().c_str());
            }
        }

        game_view.level_view.update();

        level_renderer.draw();
        graphics.update();
    }

    server.stop();

    graphics.stop();
}

int main(int argc, char *argv[]) {

    try {
        run();
    } catch (Error &ex) {
        std::cerr << "Failed with: " << ex.what() << std::endl;
    }

    return 0;
}
