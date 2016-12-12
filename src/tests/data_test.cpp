#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/messaging/loader.h"
#include "hexutil/messaging/builtin_messages.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_updater.h"
#include "hexgame/game/game_messages.h"

#include "hexview/resources/resources.h"
#include "hexview/resources/resource_loader.h"
#include "hexview/resources/resource_messages.h"


void run() {
    register_builtin_messages();
    register_game_messages();
    register_resource_messages();
    register_property_names();

    Resources resources;
    ResourceLoader loader(&resources, NULL, NULL);
    loader.load("data/resources.txt");

    Game game;
    GameUpdater game_updater(&game);
    ReceiverMessageLoader game_loader(game_updater);
    game_loader.load("data/game.txt");
}

int main(int argc, char *argv[]) {
    try {
        run();
    } catch (Error &ex) {
        BOOST_LOG_TRIVIAL(fatal) << "Failed with: " << ex.what();
    }

    return 0;
}
