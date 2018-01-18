#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/messaging/loader.h"
#include "hexutil/messaging/builtin_messages.h"

#include "hexav/resources/resource_messages.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_updater.h"
#include "hexgame/game/game_messages.h"

#include "hexview/view/view_resources.h"
#include "hexview/view/view_resource_loader.h"
#include "hexview/view/view_resource_messages.h"

#define BOOST_TEST_MODULE DataTest
#include <boost/test/included/unit_test.hpp>


namespace hex {

struct Fixture {
};

BOOST_FIXTURE_TEST_SUITE(hexgrid_test, Fixture)

BOOST_AUTO_TEST_CASE(test_resources_load) {
    register_builtin_messages();
    register_game_messages();
    register_resource_messages();
    register_view_resource_messages();
    register_property_names();

    ViewResources resources;
    ViewResourceLoader loader(&resources, NULL, NULL);
    loader.load("data/resources.txt");
}

BOOST_AUTO_TEST_CASE(test_game_load) {
    register_builtin_messages();
    register_game_messages();
    register_resource_messages();
    register_view_resource_messages();
    register_property_names();

    Game game;
    GameUpdater game_updater(&game);
    ReceiverMessageLoader game_loader(game_updater);
    game_loader.load("data/game.txt");
}

BOOST_AUTO_TEST_SUITE_END()

};
