#include "common.h"

#include "hexutil/basics/string_vector.h"

#define BOOST_TEST_MODULE StringUtilsTest
#include <boost/test/included/unit_test.hpp>


bool check_arrays(const std::vector<std::string>& expected, const std::vector<std::string>& actual) {
    BOOST_CHECK_EQUAL(expected.size(), actual.size());
    if (expected.size() == actual.size()) {
        for (unsigned int i = 0; i < expected.size(); i++) {
            BOOST_CHECK_EQUAL(expected[i], actual[i]);
            if (expected[i] != actual[i])
                return false;
        }
        return true;
    } else {
        return false;
    }
}

struct Fixture {
};

BOOST_FIXTURE_TEST_SUITE(compress_test, Fixture)

BOOST_AUTO_TEST_CASE(empty) {
    std::vector<std::string> input;
    std::vector<std::string> output;
    std::vector<std::string> expected;
    compress_string_vector(input, output);
    if (check_arrays(expected, output)) {
        std::vector<std::string> output2;
        decompress_string_vector(output, output2);
        check_arrays(input, output2);
    }
}

BOOST_AUTO_TEST_CASE(pattern_1) {
    std::vector<std::string> input;
    input.push_back("item");
    std::vector<std::string> output;
    std::vector<std::string> expected;
    expected.push_back("item");
    compress_string_vector(input, output);
    if (check_arrays(expected, output)) {
        std::vector<std::string> output2;
        decompress_string_vector(output, output2);
        check_arrays(input, output2);
    }
}

BOOST_AUTO_TEST_CASE(pattern_11) {
    std::vector<std::string> input;
    input.push_back("item");
    input.push_back("item");
    std::vector<std::string> output;
    std::vector<std::string> expected;
    expected.push_back("item");
    expected.push_back("1");
    compress_string_vector(input, output);
    if (check_arrays(expected, output)) {
        std::vector<std::string> output2;
        decompress_string_vector(output, output2);
        check_arrays(input, output2);
    }
}

BOOST_AUTO_TEST_CASE(pattern_121) {
    std::vector<std::string> input;
    input.push_back("item");
    input.push_back("other");
    input.push_back("item");
    std::vector<std::string> output;
    std::vector<std::string> expected;
    expected.push_back("item");
    expected.push_back("other");
    expected.push_back("1");
    compress_string_vector(input, output);
    if (check_arrays(expected, output)) {
        std::vector<std::string> output2;
        decompress_string_vector(output, output2);
        check_arrays(input, output2);
    }
}

BOOST_AUTO_TEST_SUITE_END()
