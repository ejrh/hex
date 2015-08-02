#include "common.h"

#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "hex/basics/error.h"
#include "hex/noise.h"
#include "hex/graphics/graphics.h"
#include "hex/messaging/serialiser.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"
#include "hex/messaging/updater.h"
#include "hex/messaging/event_pusher.h"
#include "hex/messaging/writer.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_serialisation.h"
#include "hex/game/game_updater.h"
#include "hex/game/game_arbiter.h"
#include "hex/game/game_writer.h"
#include "hex/game/movement.h"
#include "hex/networking/networking.h"
#include "hex/view/view.h"
#include "hex/view/view_updater.h"
#include "hex/view/level_renderer.h"
#include "hex/view/level_window.h"
#include "hex/chat/chat.h"

struct Options {
    bool server_mode;
    bool client_mode;
    std::string host_name;
    std::string host_addr;
};

void generate_level(Level &level, TileType *outside, TileType *water, TileType *desert, TileType *grass) {
    PerlinNoise noise(5, 5);
    PerlinNoise noise2(10, 10);

    for (int i = 0; i < level.height; i++) {
        for (int j = 0; j < level.width; j++) {
            if (i == 0 || i == level.height - 1 || j == 0 || j == level.width - 1) {
                level.tiles[i][j].type = outside;
                continue;
            }

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

    game.level.width = width;
    game.level.height = height;
    game.level.tiles.resize(width, height);
    generate_level(game.level, game.tile_types["outside"], game.tile_types["water"], game.tile_types["desert"], game.tile_types["grass"]);

    updater.receive(boost::make_shared<WrapperMessage2<int, int> >(SetLevel, game.level.width, game.level.height));
    for (int i = 0; i < game.level.height; i++) {
        Point origin(0, i);
        std::vector<std::string> data;
        for (int j = 0; j < game.level.width; j++) {
            TileType *tile_type = game.level.tiles[i][j].type;
            data.push_back(tile_type->name);
        }
        updater.receive(boost::make_shared<WrapperMessage2<Point, std::vector<std::string> > >(SetLevelData, origin, data));
    }

    updater.receive(create_message(CreateFaction, 1, "orcs", "Orc Hegemony"));
    updater.receive(create_message(CreateFaction, 2, "drow", "Great Drow Empire"));

    for (int i = 1; i <= 25; i++) {
        UnitTypeMap::iterator item = game.unit_types.begin();
        std::advance(item, rand() % game.unit_types.size());

        std::map<int, Faction *>::iterator faction_iter = game.factions.begin();
        std::advance(faction_iter, rand() % game.factions.size());
        int faction = faction_iter->second->id;

        MovementModel movement_model;
        Point p(-1, -1);
        for (int j = 0; j < 10; j++) {
            int tx = rand() % game.level.width;
            int ty = rand() % game.level.height;
            if (movement_model.admits(item->second, game.level.tiles[ty][tx].type)) {
                p = Point(tx, ty);
                break;
            }
        }

        if (p.x != -1) {
            updater.receive(create_message(CreateStack, i, p, faction));
            updater.receive(create_message(CreateUnit, i, item->second->name));
        }
    };
}

void load_resources(Resources *resources, Graphics *graphics) {
    ImageLoader image_loader(resources, graphics);
    ResourceLoader loader(resources, &image_loader);
    loader.load(std::string("data/resources.txt"));
    resources->resolve_references();
}

void save_game(const std::string& filename, Game *game) {
    std::ofstream f(filename.c_str());
    MessageWriter message_writer(f);
    GameWriter game_writer(&message_writer);
    game_writer.write(game);
}

void run(Options& options) {
    Graphics graphics;

    graphics.start();

    Resources resources;
    load_resources(&resources, &graphics);

    EventPusher event_pusher;
    Server server(9999, &event_pusher);

    Client client(&event_pusher);

    Game game;
    Updater updater(1000);
    GameArbiter arbiter(&game, &updater);
    Updater dispatcher(1000);

    GameUpdater game_updater(&game);
    updater.subscribe(&game_updater);

    GameView game_view(&game, &resources, &dispatcher);
    ViewUpdater view_updater(&game, &game_view, &resources);
    updater.subscribe(&view_updater);

    if (options.server_mode) {
        server.start();
        updater.subscribe(&server);
    }

    if (options.client_mode) {
        client.connect(options.host_addr);
        dispatcher.subscribe(&client);
    } else {
        dispatcher.subscribe(&arbiter);

        create_game(game, updater);
    }

    LevelRenderer level_renderer(&graphics, &resources, &game.level, &game_view, &game_view.level_view);
    LevelWindow level_window(graphics.width, graphics.height, &game_view.level_view, &level_renderer, &resources);

    ChatWindow chat_window(200, graphics.height, &resources, &graphics, &dispatcher);
    ChatUpdater chat_updater(&chat_window);
    updater.subscribe(&chat_updater);

    int down_pos_x = 0, down_pos_y = 0;
    bool dragging = false;

    bool running = true;
    unsigned int last_tick = 0;
    while (running) {
        SDL_Event evt;
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT
                || (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE))
                running = false;

            if (evt.type == SDL_MOUSEMOTION) {
                level_window.set_mouse_position(evt.motion.x, evt.motion.y);
            }

            if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LMASK) {
                down_pos_x = evt.motion.x;
                down_pos_y = evt.motion.y;
            } else if (evt.type == SDL_MOUSEBUTTONUP && evt.button.button == SDL_BUTTON_LMASK) {
                if (!dragging) {
                    level_window.left_click(evt.button.x, evt.button.y);
                }
                dragging = false;
            } else if (evt.type == SDL_MOUSEBUTTONUP && evt.button.button == 3) {
                if (!dragging) {
                    level_window.right_click(evt.button.x, evt.button.y);
                }
                dragging = false;
            } else if (evt.type == SDL_MOUSEMOTION && dragging) {
                level_window.shift(evt.motion.xrel, evt.motion.yrel);
            } else if (evt.type == SDL_MOUSEMOTION) {
                if (evt.motion.state == SDL_BUTTON_LMASK &&  abs(evt.motion.x - down_pos_x) > 4 && abs(evt.motion.y - down_pos_y) > 4)
                    dragging = true;
            }

            if (evt.type == SDL_KEYDOWN)
                chat_window.keypress(evt.key.keysym.sym);

            if (evt.type == SDL_TEXTINPUT) {
                chat_window.type(&evt.text);
            }

            if (evt.type == event_pusher.event_type) {
                boost::shared_ptr<Message> msg = event_pusher.get_message(evt);
                if (options.server_mode) {
                    arbiter.receive(msg);
                } else if (options.client_mode) {
                    updater.receive(msg);
                }
            }

            if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_F2) {
                save_game("save.txt", &game);
            }
        }

        game_view.level_view.update();

        level_window.draw();
        chat_window.draw();
        graphics.update();

        unsigned int tick = SDL_GetTicks();
        unsigned int tick_diff = tick - last_tick;
        unsigned int sleep_time = 40 - tick_diff;
        if (sleep_time > 25)
            sleep_time = 25;
        SDL_Delay(sleep_time);
        last_tick = tick;
    }

    server.stop();
    client.disconnect();

    graphics.stop();
}

bool parse_options(int argc, char *argv[], Options& options) {

    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("server", "run in server mode")
        ("connect", po::value<std::string>(), "connect to server")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return false;
    }

    if (vm.count("server")) {
        options.server_mode = true;
    } else {
        options.server_mode = false;
    }

    if (vm.count("connect")) {
        options.client_mode = true;
        options.host_addr = vm["connect"].as<std::string>();
    } else {
        options.client_mode = false;
    }

    return true;
}

int main(int argc, char *argv[]) {
    try {
        Options options;
        if (parse_options(argc, argv, options))
            run(options);
    } catch (Error &ex) {
        std::cerr << "Failed with: " << ex.what() << std::endl;
    }

    return 0;
}
