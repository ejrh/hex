#include "common.h"

#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "hexutil/basics/error.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/serialiser.h"
#include "hexutil/messaging/writer.h"
#include "hexutil/messaging/receiver.h"
#include "hexutil/messaging/publisher.h"
#include "hexutil/messaging/logger.h"
#include "hexutil/messaging/queue.h"
#include "hexutil/networking/networking.h"
#include "hexutil/messaging/builtin_messages.h"

#include "hexav/audio/audio.h"
#include "hexav/graphics/graphics.h"

#include "hexgame/ai/ai.h"
#include "hexgame/game/game.h"
#include "hexgame/game/game_arbiter.h"
#include "hexgame/game/game_messages.h"
#include "hexgame/game/game_updater.h"
#include "hexgame/game/game_writer.h"
#include "hexgame/game/generation/generator.h"

#include "hexview/chat/chat.h"
#include "hexview/resources/resource_loader.h"
#include "hexview/resources/resource_messages.h"
#include "hexview/view/audio_renderer.h"
#include "hexview/view/level_renderer.h"
#include "hexview/view/level_window.h"
#include "hexview/view/map_window.h"
#include "hexview/view/message_window.h"
#include "hexview/view/stack_window.h"
#include "hexview/view/status_window.h"
#include "hexview/view/player.h"
#include "hexview/view/pre_updater.h"
#include "hexview/view/unit_info_window.h"
#include "hexview/view/unit_renderer.h"
#include "hexview/view/view.h"
#include "hexview/view/view_updater.h"
#include "hexview/view/combat/battle_viewer.h"


struct Options {
    bool server_mode;
    bool client_mode;
    std::string host_name;
    std::string host_addr;
    std::string load_filename;
    int width, height;
    bool fullscreen;
};

void load_resources(Resources *resources, Graphics *graphics, Audio *audio) {
    ImageLoader image_loader(resources, graphics);
    SoundLoader sound_loader(resources, audio);
    ResourceLoader loader(resources, &image_loader, &sound_loader);
    loader.load(std::string("data/resources.txt"));
    resources->resolve_references();
}

void load_game(const std::string& filename, MessageReceiver& updater) {
    replay_messages(filename, updater);
}

void save_game(const std::string& filename, Game *game) {
    std::ofstream f(filename.c_str());
    MessageWriter message_writer(f);
    GameWriter game_writer(&message_writer);
    game_writer.write(game);
}

class NodeInterface: public MessageReceiver {
public:
    virtual ~NodeInterface() { }
    virtual void update() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void subscribe(MessageReceiver *receiver) = 0;
};

class LocalNode: public NodeInterface {
public:
    LocalNode():
            game(), game_updater(&game), publisher(1000), arbiter(&game, &publisher), dispatch_queue(1000), update_logger("Update: ") {
        publisher.subscribe(&update_logger);
        publisher.subscribe(&game_updater);
    }

    virtual void receive(Message *command) {
        dispatch_queue.receive(command);
    }

    virtual void update() {
        dispatch_queue.flush(&arbiter);
    }

    virtual void start() {
        for (auto iter = ais.begin(); iter != ais.end(); iter++) {
            Ai *ai = *iter;
            ai->start();
        }
    }

    virtual void stop() {
        for (auto iter = ais.begin(); iter != ais.end(); iter++) {
            Ai *ai = *iter;
            ai->stop();
            delete ai;
        }
    }

    virtual void subscribe(MessageReceiver *receiver) {
        publisher.subscribe(receiver);
    }

    void add_ai(const std::string& faction_type) {
        Ai *ai = new Ai(faction_type, &dispatch_queue);
        subscribe(ai->get_receiver());
        ais.push_back(ai);
    }

    MessageReceiver& get_publisher() {
        return publisher;
    }

protected:
    Game game;
    GameUpdater game_updater;
    Publisher publisher;
    GameArbiter arbiter;
    MessageQueue dispatch_queue;
    MessageLogger update_logger;
    std::vector<Ai *> ais;
};

class ServerNode: public LocalNode {
public:
    ServerNode():
            server(9999, &arbiter) {
        publisher.subscribe(&server);
    }

    virtual void start() {
        server.start();
    }

    virtual void stop() {
        server.stop();
    }

private:
    Server server;
};

class ClientNode: public NodeInterface {
public:
    ClientNode(const std::string& host_addr):
            host_addr(host_addr), update_queue(1000), client(&update_queue) {
    }

    virtual void receive(Message *command) {
        client.receive(command);
    }

    virtual void update() {
        update_queue.flush(&publisher);
    }

    virtual void start() {
        client.connect(host_addr);
    }

    virtual void stop() {
        client.disconnect();
    }

    virtual void subscribe(MessageReceiver *receiver) {
        publisher.subscribe(receiver);
    }

private:
    std::string host_addr;
    MessageQueue update_queue;
    Client client;
    Publisher publisher;
};

NodeInterface *make_node_interface(Options& options) {
    NodeInterface *node;

    if (options.client_mode) {
        node = new ClientNode(options.host_addr);
    } else if (options.server_mode) {
        node = new ServerNode();
    } else {
        node = new LocalNode();
    }

    return node;
}

class BackgroundWindow: public UiWindow {
public:
    BackgroundWindow(UiLoop *loop, Options *options, Generator *generator, Game *game, GameView *game_view,
            NodeInterface *node_interface,
            LevelRenderer *level_renderer):
        UiWindow(0, 0, 0, 0, WindowIsVisible|WindowIsActive|WindowWantsKeyboardEvents),
        loop(loop), options(options), generator(generator), game(game), game_view(game_view),
        node_interface(node_interface), level_renderer(level_renderer) { }

    bool receive_keyboard_event(SDL_Event *evt) {
        if (evt->type == SDL_QUIT
            || (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE)) {
            loop->running = false;
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
            LocalNode *local = dynamic_cast<LocalNode *>(node_interface);
            if (local != NULL) {
                MessageReceiver& updater = local->get_publisher();

                generator->mountain_level += (evt->key.keysym.mod & KMOD_SHIFT) ? 0.2 : -0.2;
                std::cerr << "mountain_level = " << generator->mountain_level << std::endl;
                generator->create_game(updater);
                updater.receive(create_message(GrantFactionView, 0, 2, true));
                updater.receive(create_message(GrantFactionControl, 0, 2, true));
                updater.receive(create_message(GrantFactionControl, 0, 3, true));
            }
            return true;
        }

        if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_F12) {
            game_view->mark_ready();
        }

        return false;
    }

    void draw(const UiContext& context) {
        game_view->update();
        node_interface->update();
    }

private:
    UiLoop *loop;
    Options *options;
    Generator *generator;
    Game *game;
    GameView *game_view;
    NodeInterface *node_interface;
    LevelRenderer *level_renderer;
};

class TopWindow: public UiWindow {
public:
    TopWindow(Graphics *graphics, Audio *audio):
            UiWindow(0, 0, 0, 0, WindowIsVisible),
            graphics(graphics), audio(audio) { }

    void draw(const UiContext& context) {
        graphics->update();
        audio->update();
    }

private:
    Graphics *graphics;
    Audio *audio;
};

void run(Options& options) {
    register_builtin_messages();
    register_game_messages();
    register_resource_messages();
    register_property_names();

    Graphics graphics;
    graphics.start("Hex", options.width, options.height, options.fullscreen);

    Resources resources;
    Audio audio(&resources);  // TODO audio should not depend on resources
    audio.start();

    load_resources(&resources, &graphics, &audio);

    NodeInterface *node_interface = make_node_interface(options);

    Player player(0, std::string("player"));

    Game game;
    GameView game_view(&game, &player, &resources, node_interface);
    PreUpdater pre_updater(&game, &game_view);
    node_interface->subscribe(&pre_updater);

    GameUpdater game_updater(&game);
    node_interface->subscribe(&game_updater);

    ViewUpdater view_updater(&game, &game_view, &resources);
    node_interface->subscribe(&view_updater);

    Generator generator;

    if (!options.client_mode) {
        LocalNode *local = static_cast<LocalNode *>(node_interface);
        local->add_ai(std::string("independent"));

        MessageReceiver& updater = local->get_publisher();
        if (options.load_filename.empty()) {
            generator.create_game(updater);
        } else {
            load_game(options.load_filename, updater);
        }

        updater.receive(create_message(GrantFactionView, 0, 2, true));
        updater.receive(create_message(GrantFactionControl, 0, 2, true));
        updater.receive(create_message(GrantFactionControl, 0, 3, true));
    }

    node_interface->start();

    int sidebar_width = StackWindow::window_width;
    int sidebar_position = graphics.width - sidebar_width;
    int map_window_height = 200;
    int stack_window_height = StackWindow::window_height;
    int status_window_height = StatusWindow::window_height;
    int message_window_height = graphics.height - map_window_height - stack_window_height - status_window_height;
    int unit_info_window_x = (graphics.width - UnitInfoWindow::unit_info_window_width) / 2;
    int unit_info_window_y = (graphics.height - UnitInfoWindow::unit_info_window_height) / 2;

    UnitRenderer unit_renderer(&graphics, &resources);
    LevelRenderer level_renderer(&graphics, &resources, &game.level, &game_view, &unit_renderer);
    AudioRenderer audio_renderer(&audio);
    LevelWindow *level_window = new LevelWindow(graphics.width - sidebar_width, graphics.height - StatusWindow::window_height, &game_view, &level_renderer, &audio_renderer, &resources);
    ChatWindow *chat_window = new ChatWindow(200, graphics.height, &resources, &graphics, node_interface);
    ChatUpdater chat_updater(chat_window);
    node_interface->subscribe(&chat_updater);

    UnitInfoWindow *unit_info_window = new UnitInfoWindow(unit_info_window_x, unit_info_window_y, UnitInfoWindow::unit_info_window_width, UnitInfoWindow::unit_info_window_height, &resources, &graphics, &game_view);
    MapWindow *map_window = new MapWindow(sidebar_position, 0, sidebar_width, map_window_height, &game_view, level_window, &graphics, &resources);
    StackWindow *stack_window = new StackWindow(sidebar_position, 200, sidebar_width, StackWindow::window_height, &resources, &graphics, &game_view, &unit_renderer, unit_info_window);
    MessageWindow *message_window = new MessageWindow(sidebar_position, map_window_height + stack_window_height, sidebar_width, message_window_height, &resources, &graphics, &game_view);
    StatusWindow *status_window = new StatusWindow(0, level_window->height, graphics.width, status_window_height, &resources, &graphics, &game_view);

    BattleViewer battle_viewer(&resources, &graphics, &audio, &game_view, &unit_renderer);
    pre_updater.battle_viewer = &battle_viewer;

    UiLoop loop(&graphics, 25);
    BackgroundWindow *bw = new BackgroundWindow(&loop, &options, &generator, &game, &game_view, node_interface, &level_renderer);
    loop.set_root_window(bw);
    bw->add_child(level_window);
    bw->add_child(map_window);
    bw->add_child(stack_window);
    bw->add_child(message_window);
    bw->add_child(status_window);
    bw->add_child(chat_window);
    bw->add_child(unit_info_window);
    TopWindow *tw = new TopWindow(&graphics, &audio);
    bw->add_child(tw);
    loop.run();

    node_interface->stop();
    delete node_interface;

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
