#include "common.h"

#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "hex/ai/ai.h"
#include "hex/ai/ai_updater.h"
#include "hex/audio/audio.h"
#include "hex/basics/error.h"
#include "hex/chat/chat.h"
#include "hex/game/game.h"
#include "hex/game/game_arbiter.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_updater.h"
#include "hex/game/game_writer.h"
#include "hex/game/generation/generator.h"
#include "hex/graphics/graphics.h"
#include "hex/messaging/event_pusher.h"
#include "hex/messaging/writer.h"
#include "hex/networking/networking.h"
#include "hex/view/level_renderer.h"
#include "hex/view/level_window.h"
#include "hex/view/map_window.h"
#include "hex/view/message_window.h"
#include "hex/view/stack_window.h"
#include "hex/view/status_window.h"
#include "hex/view/player.h"
#include "hex/view/pre_updater.h"
#include "hex/view/view.h"
#include "hex/view/view_updater.h"
#include "hex/view/combat/battle_viewer.h"


struct Options {
    bool server_mode;
    bool client_mode;
    std::string host_name;
    std::string host_addr;
    std::string load_filename;
    int width, height;
    bool fullscreen;
};

void load_resources(Resources *resources, Graphics *graphics) {
    ImageLoader image_loader(resources, graphics);
    ResourceLoader loader(resources, &image_loader);
    loader.load(std::string("data/resources.txt"));
    resources->resolve_references();
}

void load_game(const std::string& filename, Updater& updater) {
    replay_messages(filename, updater);
}

void save_game(const std::string& filename, Game *game) {
    std::ofstream f(filename.c_str());
    MessageWriter message_writer(f);
    GameWriter game_writer(&message_writer);
    game_writer.write(game);
}

class BackgroundWindow: public UiWindow {
public:
    BackgroundWindow(UiLoop *loop, Options *options, Generator *generator, Game *game, GameView *game_view, std::vector<Ai *> ais, EventPusher *event_pusher, GameArbiter *arbiter, Updater *updater, LevelRenderer *level_renderer):
        UiWindow(0, 0, 0, 0), loop(loop), options(options), generator(generator), game(game), game_view(game_view), ais(ais), event_pusher(event_pusher), arbiter(arbiter), updater(updater), level_renderer(level_renderer) { }

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
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_F5) {
            game_view->debug_mode = !game_view->debug_mode;
            level_renderer->show_hexagons = game_view->debug_mode;
            return true;
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_F6) {
            generator->mountain_level += (evt->key.keysym.mod & KMOD_SHIFT) ? 0.2 : -0.2;
            std::cerr << "mountain_level = " << generator->mountain_level << std::endl;
            generator->create_game(*updater);
            updater->receive(create_message(GrantFactionView, 0, 2, true));
            updater->receive(create_message(GrantFactionControl, 0, 2, true));
            updater->receive(create_message(GrantFactionControl, 0, 3, true));
            return true;
        }

        if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_F12) {
            game_view->mark_ready();
        }

        return false;
    }

    bool contains(int px, int py) {
        return true;
    }

    void draw() {
        game_view->update();
        for (std::vector<Ai *>::iterator iter = ais.begin(); iter != ais.end(); iter++) {
            (*iter)->update();
        }
    }

private:
    UiLoop *loop;
    Options *options;
    Generator *generator;
    Game *game;
    GameView *game_view;
    std::vector<Ai *> ais;
    EventPusher *event_pusher;
    GameArbiter *arbiter;
    Updater *updater;
    LevelRenderer *level_renderer;
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

    graphics.start("Hex", options.width, options.height, options.fullscreen);

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

    GameView game_view(&game, &player, &resources, &dispatcher);
    PreUpdater pre_updater(&game, &game_view);
    updater.subscribe(&pre_updater);

    GameUpdater game_updater(&game);
    updater.subscribe(&game_updater);

    ViewUpdater view_updater(&game, &game_view, &resources);
    updater.subscribe(&view_updater);

    std::vector<Ai *> ais;

    Generator generator;

    if (options.server_mode) {
        server.start();
        updater.subscribe(&server);
    }

    if (options.client_mode) {
        client.connect(options.host_addr);
        dispatcher.subscribe(&client);
    } else {
        Ai *independent_ai = new Ai(&game, std::string("independent"), &dispatcher);
        AiUpdater *independent_ai_updater = new AiUpdater(independent_ai);
        ais.push_back(independent_ai);

        dispatcher.subscribe(&arbiter);
        updater.subscribe(independent_ai_updater);
        if (options.load_filename.empty()) {
            generator.create_game(updater);
        } else {
            load_game(options.load_filename, updater);
        }

        updater.receive(create_message(GrantFactionView, 0, 2, true));
        updater.receive(create_message(GrantFactionControl, 0, 2, true));
        updater.receive(create_message(GrantFactionControl, 0, 3, true));
    }

    int sidebar_width = StackWindow::window_width;
    int sidebar_position = graphics.width - sidebar_width;
    int map_window_height = 200;
    int stack_window_height = StackWindow::window_height;
    int status_window_height = StatusWindow::window_height;
    int message_window_height = graphics.height - map_window_height - stack_window_height - status_window_height;

    LevelRenderer level_renderer(&graphics, &resources, &game.level, &game_view);
    LevelWindow level_window(graphics.width - sidebar_width, graphics.height - StatusWindow::window_height, &game_view, &level_renderer, &resources);
    ChatWindow chat_window(200, graphics.height, &resources, &graphics, &dispatcher);
    ChatUpdater chat_updater(&chat_window);
    updater.subscribe(&chat_updater);

    MapWindow map_window(sidebar_position, 0, sidebar_width, map_window_height, &game_view, &level_window, &graphics, &resources);
    StackWindow stack_window(sidebar_position, 200, sidebar_width, StackWindow::window_height, &resources, &graphics, &game_view, &level_renderer);
    MessageWindow message_window(sidebar_position, map_window_height + stack_window_height, sidebar_width, message_window_height, &resources, &graphics, &game_view);
    StatusWindow status_window(0, level_window.height, graphics.width, status_window_height, &resources, &graphics, &game_view);

    Audio audio(&resources);
    audio.start();

    BattleViewer battle_viewer(&resources, &graphics, &audio, &game_view, &level_renderer);
    pre_updater.battle_viewer = &battle_viewer;

    UiLoop loop(25);
    BackgroundWindow bw(&loop, &options, &generator, &game, &game_view, ais, &event_pusher, &arbiter, &updater, &level_renderer);
    loop.add_window(&bw);
    loop.add_window(&level_window);
    loop.add_window(&map_window);
    loop.add_window(&stack_window);
    loop.add_window(&message_window);
    loop.add_window(&status_window);
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
        ("connect", po::value<std::string>()->value_name("ADDRESS"), "connect to server")
        ("load", po::value<std::string>()->value_name("FILENAME"), "load game file")
        ("width", po::value<int>()->value_name("WIDTH"), "screen width")
        ("height", po::value<int>()->value_name("HEIGHT"), "screen height")
        ("fullscreen", "fullscreen mode")
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

    if (vm.count("load")) {
        options.load_filename = vm["load"].as<std::string>();
    }

    if (vm.count("fullscreen")) {
        options.fullscreen = true;
        options.width = 0;
        options.height = 0;
    } else {
        options.fullscreen = false;
        options.width = 800;
        options.height = 600;
    }

    if (vm.count("width")) {
        options.width = vm["width"].as<int>();
    }

    if (vm.count("height")) {
        options.height = vm["height"].as<int>();
    }

    return true;
}

int main(int argc, char *argv[]) {
    try {
        Options options;
        if (parse_options(argc, argv, options))
            run(options);
    } catch (Error &ex) {
        BOOST_LOG_TRIVIAL(fatal) << "Failed with: " << ex.what();
    } catch (boost::program_options::error& ex) {
        std::cout << "Error: " << ex.what();
    }

    return 0;
}
