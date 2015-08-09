#include "common.h"

#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "hex/noise.h"
#include "hex/ai/ai.h"
#include "hex/ai/ai_updater.h"
#include "hex/audio/audio.h"
#include "hex/basics/error.h"
#include "hex/basics/hexgrid.h"
#include "hex/chat/chat.h"
#include "hex/game/game.h"
#include "hex/game/game_arbiter.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_updater.h"
#include "hex/game/game_writer.h"
#include "hex/game/movement.h"
#include "hex/graphics/graphics.h"
#include "hex/messaging/event_pusher.h"
#include "hex/messaging/message.h"
#include "hex/messaging/writer.h"
#include "hex/networking/networking.h"
#include "hex/view/level_renderer.h"
#include "hex/view/level_window.h"
#include "hex/view/map_window.h"
#include "hex/view/stack_window.h"
#include "hex/view/player.h"
#include "hex/view/view.h"
#include "hex/view/view_updater.h"


struct Options {
    bool server_mode;
    bool client_mode;
    std::string host_name;
    std::string host_addr;
};

void generate_level(Level &level, TileTypeMap& types) {
    PerlinNoise noise(5, 5);
    PerlinNoise noise2(10, 10);

    for (int i = 0; i < level.height; i++) {
        for (int j = 0; j < level.width; j++) {
            Tile& tile = level.tiles[i][j];
            if (i == 0 || i == level.height - 1 || j == 0 || j == level.width - 1) {
                tile.type = types["outside"];
                continue;
            }

            float px = (float) j / level.width;
            float py = (float) i / level.height;
            if (j % 2 == 1)
                py += 0.5f / level.height;

            float value = (noise.value(px, py) + noise2.value(py, px))/6.0f;
            if (value < 0.0f)
                tile.type = types["water"];
            else if (value < 0.5f)
                tile.type = types["desert"];
            else {
                tile.type = types["grass"];
                if (value > 1.4f) {
                    tile.type = types["grass_mountain1"];
                } else if (value > 1.2f && rand() % 2) {
                    tile.type = types["grass_hill1"];
                }
            }

            if (tile.type->has_property(Roadable) && rand() % 2)
                tile.road = true;
        }
    }

    // Coalesce hills and mountains
    for (int i = 0; i < level.height; i++) {
        for (int j = 0; j < level.width; j++) {
            Point tile_pos(j, i);
            Tile& tile = level.tiles[tile_pos];
            if (tile.type == types["grass_hill1"]) {
                bool left = rand() % 2 == 0;
                int dir = left ? 5 : 1;
                Point neighbour_pos;
                get_neighbour(tile_pos, dir, &neighbour_pos);
                if (!level.contains(neighbour_pos))
                    continue;
                Tile& neighbour = level.tiles[neighbour_pos];
                if (neighbour.type == types["grass_hill1"]) {
                    tile.type = left ? types["grass_hill3"] : types["grass_hill2"];
                    neighbour.type = types["grass_hill0"];
                }
            } else if (tile.type == types["grass_mountain1"]) {
                Point neighbour_pos[6];
                get_neighbours(tile_pos, neighbour_pos);
                if (!level.contains(neighbour_pos[5]) || !level.contains(neighbour_pos[0]) || !level.contains(neighbour_pos[1]))
                    continue;
                Tile& neighbour5 = level.tiles[neighbour_pos[5]];
                Tile& neighbour0 = level.tiles[neighbour_pos[0]];
                Tile& neighbour1 = level.tiles[neighbour_pos[1]];
                if (neighbour5.type != types["grass_mountain1"] || neighbour0.type != types["grass_mountain1"] || neighbour1.type != types["grass_mountain1"])
                    continue;

                tile.type = types["grass_mountain2"];
                neighbour5.type = types["grass_mountain0"];
                neighbour0.type = types["grass_mountain0"];
                neighbour1.type = types["grass_mountain0"];
            }
        }
    }
}

void create_game(Game& game, Updater& updater) {
    int width = 57;
    int height = 48;

    replay_messages("data/tile_types.txt", updater);
    replay_messages("data/unit_types.txt", updater);
    replay_messages("data/structure_types.txt", updater);

    game.level.width = width;
    game.level.height = height;
    game.level.tiles.resize(width, height);
    generate_level(game.level, game.tile_types);

    updater.receive(boost::make_shared<WrapperMessage2<int, int> >(SetLevel, game.level.width, game.level.height));
    for (int i = 0; i < game.level.height; i++) {
        Point origin(0, i);
        std::vector<std::string> data;
        for (int j = 0; j < game.level.width; j++) {
            Tile& tile = game.level.tiles[i][j];
            std::ostringstream data_ss;
            TileType *tile_type = tile.type;
            data_ss << tile_type->name;
            if (tile.road)
                data_ss << "/r";
            data.push_back(data_ss.str());
        }
        updater.receive(boost::make_shared<WrapperMessage2<Point, std::vector<std::string> > >(SetLevelData, origin, data));
    }

    updater.receive(create_message(CreateFaction, 1, "independent", "Independent"));
    updater.receive(create_message(CreateFaction, 2, "orcs", "Orc Hegemony"));
    updater.receive(create_message(CreateFaction, 3, "drow", "Great Drow Empire"));

    updater.receive(create_message(GrantFactionView, 0, 2, true));
    updater.receive(create_message(GrantFactionControl, 0, 2, true));

    for (int i = 1; i <= 40; i++) {
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
            int num = (rand() % 5) + 1;
            for (int j = 0; j < num; j++) {
                updater.receive(create_message(CreateUnit, i, item->second->name));
            }
        }
    };

    // Add towers
    for (int i = 0; i < game.level.height; i++) {
        for (int j = 0; j < game.level.width; j++) {
            Point tile_pos(j, i);
            Tile& tile = game.level.tiles[tile_pos];
            if (!tile.type->has_property(Roadable))
                continue;
            Point neighbour_pos[6];
            get_neighbours(tile_pos, neighbour_pos);
            int water_count = 0;
            for (int i = 0; i < 6; i++) {
                if (!game.level.contains(neighbour_pos[i]))
                    continue;
                Tile& neighbour = game.level.tiles[neighbour_pos[i]];
                if (neighbour.type == game.tile_types["water"])
                    water_count++;
            }
            if (water_count >= 3 && rand() % 4 == 0) {
                std::map<int, Faction *>::iterator faction_iter = game.factions.begin();
                std::advance(faction_iter, rand() % game.factions.size());
                int faction = faction_iter->second->id;

                updater.receive(create_message(CreateStructure, tile_pos, "tower", faction));
            }
        }
    }
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

class BackgroundWindow: public UiWindow {
public:
    BackgroundWindow(UiLoop *loop, Options *options, Game *game, GameView *game_view, Ai *independent_ai, EventPusher *event_pusher, GameArbiter *arbiter, Updater *updater):
        UiWindow(0, 0, 0, 0), loop(loop), options(options), game(game), game_view(game_view), independent_ai(independent_ai), event_pusher(event_pusher), arbiter(arbiter), updater(updater) { }

    bool receive_event(SDL_Event *evt) {
        if (evt->type == SDL_QUIT
            || (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE)) {
            loop->running = false;
            return true;
        } else if (evt->type == event_pusher->event_type) {
            boost::shared_ptr<Message> msg = event_pusher->get_message(*evt);
            if (options->server_mode) {
                arbiter->receive(msg);
            } else if (options->client_mode) {
                updater->receive(msg);
            }
            return true;
        }

        if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_F2) {
            save_game("save.txt", game);
            return true;
        }

        return false;
    }

    bool contains(int px, int py) {
        return true;
    }

    void draw() {
        game_view->update();
        independent_ai->update();
    }

private:
    UiLoop *loop;
    Options *options;
    Game *game;
    GameView *game_view;
    Ai *independent_ai;
    EventPusher *event_pusher;
    GameArbiter *arbiter;
    Updater *updater;
};

class TopWindow: public UiWindow {
public:
    TopWindow(Graphics *graphics, Audio *audio): UiWindow(0, 0, 0, 0), graphics(graphics), audio(audio) { }

    void draw() {
        graphics->update();
        audio->update();
    }

private:
    Graphics *graphics;
    Audio *audio;
};

void run(Options& options) {
    Graphics graphics;

    graphics.start();

    Resources resources;
    load_resources(&resources, &graphics);

    EventPusher event_pusher;
    Server server(9999, &event_pusher);

    Client client(&event_pusher);

    Player player(0, std::string("player"));

    Game game;
    Updater updater(1000);
    GameArbiter arbiter(&game, &updater);
    Updater dispatcher(1000);

    GameUpdater game_updater(&game);
    updater.subscribe(&game_updater);

    GameView game_view(&game, &player, &resources, &dispatcher);
    ViewUpdater view_updater(&game, &game_view, &resources);
    updater.subscribe(&view_updater);

    Ai independent_ai(&game, std::string("independent"), &dispatcher);
    AiUpdater independent_ai_updater(&independent_ai);

    if (options.server_mode) {
        server.start();
        updater.subscribe(&server);
    }

    if (options.client_mode) {
        client.connect(options.host_addr);
        dispatcher.subscribe(&client);
    } else {
        dispatcher.subscribe(&arbiter);
        updater.subscribe(&independent_ai_updater);
        create_game(game, updater);
    }

    LevelRenderer level_renderer(&graphics, &resources, &game.level, &game_view);
    LevelWindow level_window(graphics.width - StackWindow::window_width, graphics.height, &game_view, &level_renderer, &resources);
    ChatWindow chat_window(200, graphics.height, &resources, &graphics, &dispatcher);
    ChatUpdater chat_updater(&chat_window);
    updater.subscribe(&chat_updater);

    MapWindow map_window(graphics.width - StackWindow::window_width, 0, StackWindow::window_width, 200, &game_view, &level_window, &graphics, &resources);
    StackWindow stack_window(graphics.width - StackWindow::window_width, 200, StackWindow::window_width, StackWindow::window_height, &resources, &graphics, &game_view, &level_renderer);

    Audio audio(&resources);
    audio.start();

    UiLoop loop(25);
    BackgroundWindow bw(&loop, &options, &game, &game_view, &independent_ai, &event_pusher, &arbiter, &updater);
    loop.add_window(&bw);
    loop.add_window(&level_window);
    loop.add_window(&map_window);
    loop.add_window(&stack_window);
    loop.add_window(&chat_window);
    TopWindow tw(&graphics, &audio);
    loop.add_window(&tw);
    loop.run();

    server.stop();
    client.disconnect();

    audio.stop();
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
