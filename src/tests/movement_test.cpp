#include "common.h"

#include "hex/basics/error.h"
#include "hex/game/game.h"
#include "hex/game/game_arbiter.h"
#include "hex/game/game_messages.h"
#include "hex/game/game_serialisation.h"
#include "hex/game/game_updater.h"
#include "hex/game/game_writer.h"
#include "hex/messaging/message.h"
#include "hex/messaging/checksum.h"
#include "hex/messaging/updater.h"
#include "hex/messaging/writer.h"

#define BOOST_TEST_MODULE MovementTest
#include <boost/test/included/unit_test.hpp>


#define LEVEL_WIDTH 8
#define LEVEL_HEIGHT 8

#define STACK1_POS Point(2,2)
#define STEP_POS Point(2,3)
#define STACK2_POS Point(2,4)
#define EMPTY_POS Point(1,3)

void create_game(MessageReceiver& updater) {
    TileType grass_type;
    grass_type.name = "grass";
    grass_type.properties.insert(Walkable);
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
    cat_type.abilities.insert(Walking);
    cat_type.moves = 24;
    cat_type.sight = 5;
    updater.receive(create_message(CreateUnitType, cat_type));

    UnitType mouse_type;
    mouse_type.name = "mouse";
    mouse_type.abilities.insert(Walking);
    mouse_type.moves = 20;
    mouse_type.sight = 4;
    updater.receive(create_message(CreateUnitType, mouse_type));

    updater.receive(create_message(CreateFaction, 1, "independent", "Independent"));

    updater.receive(create_message(CreateStack, 1, STACK1_POS, 1));
    updater.receive(create_message(CreateUnit, 1, "cat"));
    updater.receive(create_message(CreateUnit, 1, "mouse"));

    updater.receive(create_message(CreateStack, 2, STACK2_POS, 1));
    updater.receive(create_message(CreateUnit, 2, "mouse"));

    updater.receive(create_message(TurnBegin, 1));
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

        path_to_empty.push_back(STEP_POS);
        path_to_empty.push_back(EMPTY_POS);

        path_to_stack2.push_back(STEP_POS);
        path_to_stack2.push_back(STACK2_POS);
    }

    Game game;
    GameUpdater game_updater;
    MessageWriter writer;
    Updater updater;
    GameArbiter arbiter;

    Path path_to_empty;
    Path path_to_stack2;
};

BOOST_FIXTURE_TEST_SUITE(s, Fixture)

BOOST_AUTO_TEST_CASE(simple_movement) {
    int stack_id = 1;
    IntSet selected_units;
    selected_units.insert(0);
    selected_units.insert(1);
    Path path = path_to_empty;
    int target_id = 0;

    unsigned long pre_checksum = game_checksum(game);
    arbiter.receive(create_message(UnitMove, stack_id, selected_units, path, target_id));
    unsigned long post_checksum = game_checksum(game);

    BOOST_CHECK_NE(pre_checksum, post_checksum);
    BOOST_CHECK_EQUAL(game.stacks[1]->position, Point(1,3));
}

BOOST_AUTO_TEST_CASE(split_movement) {
    int stack_id = 1;
    IntSet selected_units;
    selected_units.insert(0);
    Path path = path_to_empty;
    int target_id = 0;

    unsigned long pre_checksum = game_checksum(game);
    arbiter.receive(create_message(UnitMove, stack_id, selected_units, path, target_id));
    unsigned long post_checksum = game_checksum(game);

    BOOST_CHECK_NE(pre_checksum, post_checksum);
    BOOST_CHECK_EQUAL(game.stacks[1]->position, Point(2,2));
    BOOST_CHECK_EQUAL(game.stacks[1]->units.size(), 1);
    BOOST_CHECK_EQUAL(game.stacks[3]->position, Point(1,3));
    BOOST_CHECK_EQUAL(game.stacks[3]->units.size(), 1);
}

BOOST_AUTO_TEST_CASE(split_and_merge_movement_0) {
    int stack_id = 1;
    IntSet selected_units;
    selected_units.insert(0);
    Path path = path_to_stack2;
    int target_id = 2;

    unsigned long pre_checksum = game_checksum(game);
    arbiter.receive(create_message(UnitMove, stack_id, selected_units, path, target_id));
    unsigned long post_checksum = game_checksum(game);

    BOOST_CHECK_NE(pre_checksum, post_checksum);
    BOOST_CHECK_EQUAL(game.stacks[1]->position, Point(2,2));
    BOOST_CHECK_EQUAL(game.stacks[1]->units.size(), 1);
    BOOST_CHECK_EQUAL(game.stacks[2]->position, Point(2,4));
    BOOST_CHECK_EQUAL(game.stacks[2]->units.size(), 2);
}

BOOST_AUTO_TEST_CASE(split_and_merge_movement_1) {
    int stack_id = 1;
    IntSet selected_units;
    selected_units.insert(1);
    Path path = path_to_stack2;
    int target_id = 2;

    unsigned long pre_checksum = game_checksum(game);
    arbiter.receive(create_message(UnitMove, stack_id, selected_units, path, target_id));
    unsigned long post_checksum = game_checksum(game);

    BOOST_CHECK_NE(pre_checksum, post_checksum);
    BOOST_CHECK_EQUAL(game.stacks[1]->position, Point(2,2));
    BOOST_CHECK_EQUAL(game.stacks[1]->units.size(), 1);
    BOOST_CHECK_EQUAL(game.stacks[2]->position, Point(2,4));
    BOOST_CHECK_EQUAL(game.stacks[2]->units.size(), 2);
}

BOOST_AUTO_TEST_CASE(merge_movement) {
    int stack_id = 1;
    IntSet selected_units;
    selected_units.insert(0);
    selected_units.insert(1);
    Path path = path_to_stack2;
    int target_id = 2;

    unsigned long pre_checksum = game_checksum(game);
    arbiter.receive(create_message(UnitMove, stack_id, selected_units, path, target_id));
    unsigned long post_checksum = game_checksum(game);

    BOOST_CHECK_NE(pre_checksum, post_checksum);
    BOOST_CHECK(game.get_stack(1) == NULL);
    BOOST_CHECK_EQUAL(game.stacks[2]->position, Point(2,4));
    BOOST_CHECK_EQUAL(game.stacks[2]->units.size(), 3);
}

BOOST_AUTO_TEST_CASE(invalid_stack) {
    int stack_id = 7;
    IntSet selected_units;
    selected_units.insert(0);
    selected_units.insert(1);
    Path path = path_to_stack2;
    int target_id = 2;

    unsigned long pre_checksum = game_checksum(game);
    arbiter.receive(create_message(UnitMove, stack_id, selected_units, path, target_id));
    unsigned long post_checksum = game_checksum(game);

    BOOST_CHECK_EQUAL(pre_checksum, post_checksum);
}

BOOST_AUTO_TEST_CASE(invalid_unit_selection) {
    int stack_id = 1;
    IntSet selected_units;
    selected_units.insert(7);
    Path path = path_to_stack2;
    int target_id = 2;

    unsigned long pre_checksum = game_checksum(game);
    arbiter.receive(create_message(UnitMove, stack_id, selected_units, path, target_id));
    unsigned long post_checksum = game_checksum(game);

    BOOST_CHECK_EQUAL(pre_checksum, post_checksum);
}

BOOST_AUTO_TEST_SUITE_END()
