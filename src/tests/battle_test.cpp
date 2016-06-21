#include "common.h"

#include "hex/basics/error.h"
#include "hex/game/game.h"
#include "hex/game/game_arbiter.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_serialisation.h"
#include "hex/game/game_updater.h"
#include "hex/game/game_writer.h"
#include "hex/game/combat/combat.h"
#include "hex/messaging/message.h"
#include "hex/messaging/checksum.h"
#include "hex/messaging/updater.h"
#include "hex/messaging/writer.h"

#define BOOST_TEST_MODULE BattleTest
#include <boost/test/included/unit_test.hpp>


#define LEVEL_WIDTH 8
#define LEVEL_HEIGHT 8

#define STACK1_POS Point(2,2)
#define STACK2_POS Point(2,3)

void create_game(MessageReceiver& updater) {
    TileType grass_type;
    grass_type.name = "grass";
    grass_type.properties[Walkable] = 1;
    updater.receive(create_message(CreateTileType, grass_type));

    TileType wall_type;
    wall_type.name = "wall";
    updater.receive(create_message(CreateTileType, wall_type));

    updater.receive(create_message(SetLevel, LEVEL_WIDTH, LEVEL_HEIGHT));
    for (int i = 0; i < LEVEL_HEIGHT; i++) {
        Point offset(0, i);
        std::vector<std::string> data;
        for (int j = 0; j < LEVEL_WIDTH; j++) {
            if (j == 3 && i >= 3)
                data.push_back("wall");
            else
                data.push_back("grass");
        }
        updater.receive(create_message(SetLevelData, offset, data));
    }

    UnitType cat_type;
    cat_type.name = "cat";
    cat_type.properties[Attack] = 5;
    cat_type.properties[Defence] = 3;
    cat_type.properties[Damage] = 2;
    cat_type.properties[Health] = 6;
    cat_type.properties[Strike] = 1;
    cat_type.properties[Charge] = 1;
    updater.receive(create_message(CreateUnitType, cat_type));

    UnitType mouse_type;
    mouse_type.name = "mouse";
    mouse_type.properties[Attack] = 2;
    mouse_type.properties[Defence] = 5;
    mouse_type.properties[Damage] = 1;
    mouse_type.properties[Health] = 4;
    mouse_type.properties[Strike] = 1;
    mouse_type.properties[Healing] = 1;
    updater.receive(create_message(CreateUnitType, mouse_type));

    updater.receive(create_message(CreateFaction, 1, "cats", "Cats"));
    updater.receive(create_message(CreateFaction, 2, "mice", "Mice"));

    updater.receive(create_message(CreateStack, 1, STACK1_POS, 1));
    updater.receive(create_message(CreateUnit, 1, "cat"));

    updater.receive(create_message(CreateStack, 2, STACK2_POS, 2));
    updater.receive(create_message(CreateUnit, 2, "mouse"));
    updater.receive(create_message(CreateUnit, 2, "mouse"));
}

unsigned long game_checksum(Game& game) {
    MessageChecksum checksum;
    GameWriter writer(&checksum);
    writer.write(&game);
    return checksum.checksum;
}

struct Fixture {
    Fixture():
             game_updater(&game), writer(std::cout), updater(1000), arbiter(&game, &updater) {
        updater.subscribe(&game_updater);
        create_game(updater);
        updater.subscribe(&writer);
    }

    Game game;
    GameUpdater game_updater;
    MessageWriter writer;
    Updater updater;
    GameArbiter arbiter;
};

BOOST_FIXTURE_TEST_SUITE(s, Fixture)

BOOST_AUTO_TEST_CASE(simple_battle) {
    srand(time(NULL));
    Battle battle(&game, STACK2_POS, STACK1_POS);
    battle.run();
    battle.commit();
}

BOOST_AUTO_TEST_SUITE_END()
