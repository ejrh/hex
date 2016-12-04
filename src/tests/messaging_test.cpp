#include "common.h"

#include "hexutil/messaging/receiver.h"
#include "hexutil/messaging/queue.h"
#include "hexutil/messaging/builtin_messages.h"

#include "hexgame/game/game_messages.h"

#define BOOST_TEST_MODULE MessagingTest
#include <boost/test/included/unit_test.hpp>

struct Fixture {
    Fixture() {
        register_builtin_messages();
        register_game_messages();
    }

    std::vector<boost::shared_ptr<Message> > parse_messages(const char *input) {
        MessageQueue collector(1000);
        std::istringstream is(input);
        replay_messages(is, collector, "input");
        return collector.get_queue();
   }
};

BOOST_FIXTURE_TEST_SUITE(messaging_test, Fixture)

BOOST_AUTO_TEST_CASE(empty_stream) {
    static const char *input = "";
    std::vector<boost::shared_ptr<Message> > messages = parse_messages(input);
    BOOST_CHECK_EQUAL(messages.size(), 0);

    input = "   ";
    messages = parse_messages(input);
    BOOST_CHECK_EQUAL(messages.size(), 0);

    input = "# comment";
    messages = parse_messages(input);
    BOOST_CHECK_EQUAL(messages.size(), 0);
}

BOOST_AUTO_TEST_CASE(simple_message) {
    static const char *input = "StreamClose()";
    std::vector<boost::shared_ptr<Message> > messages = parse_messages(input);
    BOOST_REQUIRE_EQUAL(messages.size(), 1);
    BOOST_CHECK(equal(messages[0], create_message(StreamClose)));

    input = "     StreamClose();";
    messages = parse_messages(input);
    BOOST_REQUIRE_EQUAL(messages.size(), 1);
    BOOST_CHECK(equal(messages[0], create_message(StreamClose)));
}

BOOST_AUTO_TEST_CASE(two_messages) {
    static const char *input = "StreamClose()\nStreamClose()";
    std::vector<boost::shared_ptr<Message> > messages = parse_messages(input);
    BOOST_REQUIRE_EQUAL(messages.size(), 2);
    BOOST_CHECK(equal(messages[0], create_message(StreamClose)));
    BOOST_CHECK(equal(messages[1], create_message(StreamClose)));

    input = "     StreamClose();  StreamClose()";
    messages = parse_messages(input);
    BOOST_REQUIRE_EQUAL(messages.size(), 2);
    BOOST_CHECK(equal(messages[0], create_message(StreamClose)));
    BOOST_CHECK(equal(messages[1], create_message(StreamClose)));
}

BOOST_AUTO_TEST_CASE(message_with_string) {
    boost::shared_ptr<Message> expected = create_message(StreamOpen, "hello");

    static const char *input = "StreamOpen( hello )";
    std::vector<boost::shared_ptr<Message> > messages = parse_messages(input);
    BOOST_REQUIRE_EQUAL(messages.size(), 1);
    BOOST_CHECK(equal(messages[0], expected));

    input = "StreamOpen( \"hello\" )";
    messages = parse_messages(input);
    BOOST_REQUIRE_EQUAL(messages.size(), 1);
    BOOST_CHECK(equal(messages[0], expected));

    expected = create_message(StreamOpen, "  hello\n there\"");

    input = "StreamOpen( \"  hello\\n there\\\"\" )";
    messages = parse_messages(input);
    BOOST_REQUIRE_EQUAL(messages.size(), 1);
    BOOST_CHECK(equal(messages[0], expected));
}

BOOST_AUTO_TEST_CASE(message_with_vector) {
    CompressableStringVector v;
    v.push_back("hello");
    boost::shared_ptr<Message> expected = create_message(SetLevelData, Point(1,2), v);

    static const char *input = "SetLevelData((1,2), [hello])";
    std::vector<boost::shared_ptr<Message> > messages = parse_messages(input);
    BOOST_REQUIRE_EQUAL(messages.size(), 1);
    BOOST_CHECK(equal(messages[0], expected));

    input = "SetLevelData((1,2), [hello, ])";
    messages = parse_messages(input);
    BOOST_REQUIRE_EQUAL(messages.size(), 1);
    BOOST_CHECK(equal(messages[0], expected));

}

BOOST_AUTO_TEST_SUITE_END()
