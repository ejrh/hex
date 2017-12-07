#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/checksum.h"
#include "hexutil/messaging/publisher.h"
#include "hexutil/messaging/queue.h"
#include "hexutil/messaging/writer.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_arbiter.h"
#include "hexgame/game/game_messages.h"
#include "hexgame/game/game_updater.h"
#include "hexgame/game/game_writer.h"
#include "hexgame/game/movement/movement.h"

#define BOOST_TEST_MODULE MovementTest
#include <boost/test/included/unit_test.hpp>


#define LEVEL_WIDTH 8
#define LEVEL_HEIGHT 8

#define STACK1_POS Point(2,2)
#define STEP_POS Point(2,3)
#define STACK2_POS Point(2,4)
#define EMPTY_POS Point(1,3)
#define STACK3_POS Point(3,4)

void create_game(MessageReceiver& updater) {
    TileType wall_type;
    wall_type.name = "wall";
    updater.receive(create_message(CreateTileType, wall_type));

    TileType grass_type;
    grass_type.name = "grass";
    updater.receive(create_message(CreateTileType, grass_type));

    FeatureType wall_feature_type;
    wall_feature_type.name = "wall";
    updater.receive(create_message(CreateFeatureType, wall_feature_type));

    FeatureType open_feature_type;
    open_feature_type.name = "open";
    open_feature_type.properties[Walkable] = 1;
    updater.receive(create_message(CreateFeatureType, open_feature_type));

    updater.receive(create_message(SetLevel, LEVEL_WIDTH, LEVEL_HEIGHT));
    for (int i = 0; i < LEVEL_HEIGHT; i++) {
        Point offset(0, i);
        CompressableStringVector data;
        CompressableStringVector feature_data;
        for (int j = 0; j < LEVEL_WIDTH; j++) {
            if (j == 3 && i >= 3) {
                data.push_back("wall");
                feature_data.push_back("wall");
            } else {
                data.push_back("grass");
                feature_data.push_back("open");
            }
        }
        updater.receive(create_message(SetLevelData, offset, data, feature_data));
    }

    UnitType cat_type;
    cat_type.name = "cat";
    cat_type.properties[Walking] = 1;
    cat_type.properties[Moves] = 24;
    cat_type.properties[Sight] = 5;
    updater.receive(create_message(CreateUnitType, cat_type));

    UnitType mouse_type;
    mouse_type.name = "mouse";
    mouse_type.properties[Walking] = 1;
    mouse_type.properties[Moves] = 20;
    mouse_type.properties[Sight] = 4;
    updater.receive(create_message(CreateUnitType, mouse_type));

    updater.receive(create_message(CreateFaction, 1, "independent", "Independent"));

    updater.receive(create_message(CreateStack, 1, STACK1_POS, 1));
    updater.receive(create_message(CreateUnit, 1, "cat"));
    updater.receive(create_message(CreateUnit, 1, "mouse"));

    updater.receive(create_message(CreateStack, 2, STACK2_POS, 1));
    updater.receive(create_message(CreateUnit, 2, "mouse"));

    updater.receive(create_message(CreateStack, 3, STACK3_POS, 1));
    for (int i = 0; i < MAX_UNITS - 1; i++) {
        updater.receive(create_message(CreateUnit, 3, "mouse"));
    }

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
             game_updater(&game), writer(std::cout), updater(1000), arbiter(&game, &updater), collector(1000) {
        register_game_messages();
        register_property_names();

        updater.subscribe(&game_updater);
        create_game(updater);
        updater.subscribe(&writer);
        updater.subscribe(&collector);

        path_to_empty.push_back(STEP_POS);
        path_to_empty.push_back(EMPTY_POS);

        path_to_stack2.push_back(STEP_POS);
        path_to_stack2.push_back(STACK2_POS);

        path_to_stack3.push_back(STEP_POS);
        path_to_stack3.push_back(STACK3_POS);
    }

    void check_received(boost::shared_ptr<Message> msg) {
        const auto& msgs = collector.get_queue();
        for (auto iter = msgs.begin(); iter != msgs.end(); iter++) {
            msg->id = (*iter)->id;
            msg->origin = (*iter)->origin;
            if (equal(msg, *iter))
                return;
        }
        std::ostringstream err_msg;
        err_msg << "Message not received: " << msg.get();
        BOOST_ERROR(err_msg.str());
    }

    Game game;
    GameUpdater game_updater;
    MessageWriter writer;
    Publisher updater;
    GameArbiter arbiter;
    MessageQueue collector;

    Path path_to_empty;
    Path path_to_stack2;
    Path path_to_stack3;
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
    BOOST_CHECK_EQUAL(game.stacks.get(1)->position, Point(1,3));
    for (auto iter = path.begin(); iter != path.end(); iter++)
        check_received(create_message(MoveUnits, stack_id, selected_units, *iter));
    check_received(create_message(TransferUnits, stack_id, selected_units, path, stack_id));
}

BOOST_AUTO_TEST_CASE(insufficient_points_1) {
    int stack_id = 1;
    IntSet selected_units;
    selected_units.insert(0);
    selected_units.insert(1);
    Path path = path_to_empty;
    int target_id = 0;
    UnitStack::pointer stack = game.stacks.get(stack_id);
    stack->units[0]->properties[Moves] = 1;
    stack->units[1]->properties[Moves] = 1;

    unsigned long pre_checksum = game_checksum(game);
    arbiter.receive(create_message(UnitMove, stack_id, selected_units, path, target_id));
    unsigned long post_checksum = game_checksum(game);

    BOOST_CHECK_EQUAL(pre_checksum, post_checksum);
}

BOOST_AUTO_TEST_CASE(insufficient_points_2) {
    int stack_id = 1;
    IntSet selected_units;
    selected_units.insert(0);
    selected_units.insert(1);
    Path path = path_to_empty;
    int target_id = 0;
    UnitStack::pointer stack = game.stacks.get(stack_id);
    Unit::pointer unit0 = stack->units[0];
    Unit::pointer unit1 = stack->units[0];
    unit0->properties[Moves] = 5;
    unit1->properties[Moves] = 5;

    unsigned long pre_checksum = game_checksum(game);
    arbiter.receive(create_message(UnitMove, stack_id, selected_units, path, target_id));
    unsigned long post_checksum = game_checksum(game);

    Path short_path = path;
    short_path.resize(1);

    BOOST_CHECK_NE(pre_checksum, post_checksum);
    BOOST_CHECK_EQUAL(stack->position, short_path[0]);
    for (auto iter = short_path.begin(); iter != short_path.end(); iter++)
        check_received(create_message(MoveUnits, stack_id, selected_units, *iter));
    check_received(create_message(TransferUnits, stack_id, selected_units, short_path, stack_id));
    BOOST_CHECK_EQUAL(unit0->properties[Moves], 1);
    BOOST_CHECK_EQUAL(unit1->properties[Moves], 1);
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
    for (auto iter = path.begin(); iter != path.end(); iter++)
        check_received(create_message(MoveUnits, stack_id, selected_units, *iter));
    check_received(create_message(CreateStack, 4, EMPTY_POS, 1));
    check_received(create_message(TransferUnits, stack_id, selected_units, path, 4));
    BOOST_CHECK_EQUAL(game.stacks.get(1)->position, Point(2,2));
    BOOST_CHECK_EQUAL(game.stacks.get(1)->units.size(), 1);
    BOOST_CHECK_EQUAL(game.stacks.get(4)->position, Point(1,3));
    BOOST_CHECK_EQUAL(game.stacks.get(4)->units.size(), 1);
}

BOOST_AUTO_TEST_CASE(split_movement_2) {
    int stack_id = 1;
    IntSet selected_units;
    selected_units.insert(0);
    Path path = path_to_empty;
    int target_id = 0;
    UnitStack::pointer stack = game.stacks.get(stack_id);
    Unit::pointer unit1 = stack->units[1];
    unit1->properties[Moves] = 1;

    unsigned long pre_checksum = game_checksum(game);
    arbiter.receive(create_message(UnitMove, stack_id, selected_units, path, target_id));
    unsigned long post_checksum = game_checksum(game);

    BOOST_CHECK_NE(pre_checksum, post_checksum);
    for (auto iter = path.begin(); iter != path.end(); iter++)
        check_received(create_message(MoveUnits, stack_id, selected_units, *iter));
    check_received(create_message(CreateStack, 4, EMPTY_POS, 1));
    check_received(create_message(TransferUnits, stack_id, selected_units, path, 4));
    BOOST_CHECK_EQUAL(stack->position, Point(2,2));
    BOOST_CHECK_EQUAL(stack->units.size(), 1);
    BOOST_CHECK_EQUAL(game.stacks.get(4)->position, Point(1,3));
    BOOST_CHECK_EQUAL(game.stacks.get(4)->units.size(), 1);
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
    BOOST_CHECK_EQUAL(game.stacks.get(1)->position, Point(2,2));
    BOOST_CHECK_EQUAL(game.stacks.get(1)->units.size(), 1);
    BOOST_CHECK_EQUAL(game.stacks.get(2)->position, Point(2,4));
    BOOST_CHECK_EQUAL(game.stacks.get(2)->units.size(), 2);
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
    BOOST_CHECK_EQUAL(game.stacks.get(1)->position, Point(2,2));
    BOOST_CHECK_EQUAL(game.stacks.get(1)->units.size(), 1);
    BOOST_CHECK_EQUAL(game.stacks.get(2)->position, Point(2,4));
    BOOST_CHECK_EQUAL(game.stacks.get(2)->units.size(), 2);
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
    check_received(create_message(DestroyStack, 1));
    BOOST_CHECK(!game.stacks.find(1));
    BOOST_CHECK_EQUAL(game.stacks.get(2)->position, Point(2,4));
    BOOST_CHECK_EQUAL(game.stacks.get(2)->units.size(), 3);
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

BOOST_AUTO_TEST_CASE(movement_model_can_enter) {
    MovementModel model(&game.level);
    bool result = model.can_enter(*game.stacks.get(1), STACK2_POS);
    BOOST_CHECK_EQUAL(result, true);
    result = model.can_enter(*game.stacks.get(1), STACK3_POS);
    BOOST_CHECK_EQUAL(result, false);
}

BOOST_AUTO_TEST_CASE(movement_model_cost_to) {
    MovementModel model(&game.level);
    int result = model.cost_to(*game.stacks.get(1), STACK2_POS);
    BOOST_CHECK_EQUAL(result, 8);
    result = model.cost_to(*game.stacks.get(1), STACK3_POS);
    BOOST_CHECK_EQUAL(result, INT_MAX);
}

BOOST_AUTO_TEST_CASE(movement_model_check_path) {
    MovementModel model(&game.level);
    int result = model.check_path(*game.stacks.get(1), path_to_empty);
    BOOST_CHECK_EQUAL(result, 2);
    result = model.check_path(*game.stacks.get(1), path_to_stack3);
    BOOST_CHECK_EQUAL(result, 1);
    result = model.check_path(*game.stacks.get(3), path_to_stack3);
    BOOST_CHECK_EQUAL(result, 0);
}

BOOST_AUTO_TEST_SUITE_END()
