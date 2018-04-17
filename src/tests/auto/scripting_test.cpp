#include "common.h"

#include "hexutil/messaging/builtin_messages.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"
#include "hexutil/scripting/scripting.h"

#include "hexgame/game/game_messages.h"

#define BOOST_TEST_MODULE ScriptingTest
#include <boost/test/included/unit_test.hpp>


namespace hex {

class ScriptLoader: public MessageReceiver {
public:
    ScriptLoader(AtomMap<Script> *scripts): scripts(scripts) {
    }

    void receive(Message *msg) {
        switch (msg->type) {
            case DefineScript: {
                auto upd = dynamic_cast<DefineScriptMessage *>(msg);
                define_script(upd->data1, upd->data2, upd->data3);
            } break;
        }
    }

    void define_script(const std::string& name, Term *parameters, Term *instruction) {
        Script::pointer script = compile_script(name, parameters, instruction);
        BOOST_LOG_TRIVIAL(info) << "Compiled script: " << script->signature();
        scripts->put_and_warn(name, script);
    }

private:
    AtomMap<Script> *scripts;
};

struct Fixture {
    Fixture(): scripts("scripts"), VarX("VarX"), VarY("VarY"), VarZ("VarZ") {
        register_builtin_messages();
        register_builtin_interpreters();
    }

    void play_message(const char *input) {
        ScriptLoader loader(&scripts);
        std::istringstream is(input);
        replay_messages(is, loader, "input");
    }

public:
    AtomMap<Script> scripts;
    Atom VarX, VarY, VarZ;
};

BOOST_FIXTURE_TEST_SUITE(scripting_test, Fixture)

BOOST_AUTO_TEST_CASE(set_var) {
    play_message("DefineScript(t, [SetVariable(VarX, \"xyz\")])");

    Execution execution(&scripts);
    execution.variables.set<std::string>(VarX, "abc");
    std::string var_value = execution.variables.get(VarX);
    BOOST_CHECK_EQUAL(var_value, "abc");

    execution.run("t");
    std::string var_value2 = execution.variables.get(VarX);
    BOOST_CHECK_EQUAL(var_value2, "xyz");
}

BOOST_AUTO_TEST_CASE(unknown_var) {
    play_message("DefineScript(t, [SetVariable(VarX, $Unknown)])");

    Execution execution(&scripts);
    execution.run("t");
    int var_value = execution.variables.get(VarX);
    BOOST_CHECK_EQUAL(var_value, 0);
}

BOOST_AUTO_TEST_CASE(call_script) {
    play_message("DefineScript(u, [SetVariable(VarX, \"xyz\")])");
    play_message("DefineScript(t, [SetVariable(VarX, \"abc\"), Call(u)])");

    Execution execution(&scripts);
    execution.run("t");
    std::string var_value = execution.variables.get(VarX);
    BOOST_CHECK_EQUAL(var_value, "xyz");
}

BOOST_AUTO_TEST_CASE(if_match) {
    play_message("DefineScript(t, ["
        "SetVariable(VarX, 32),"
        "SetVariable(StrVar, \"abc\"),"
        "If(Match($StrVar, \"a.+\"), SetVariable(VarX, 40)),"
        "SetVariable(VarY, 4),"
        "If(Match($StrVar, \"x.+\"), SetVariable(VarY, 7))])");

    Execution execution(&scripts);
    execution.run("t");
    int var_value = execution.variables.get(VarX);
    BOOST_CHECK_EQUAL(var_value, 40);
    var_value = execution.variables.get(VarY);
    BOOST_CHECK_EQUAL(var_value, 4);
}

BOOST_AUTO_TEST_CASE(choose) {
    play_message("DefineScript(t, ["
        "SetVariable(VarX, 6),"
        "SetVariable(VarY, Choose($VarX, [abc, def])),"
        "SetVariable(VarX, 7),"
        "SetVariable(VarZ, Choose($VarX, [abc, def]))])");

    Execution execution(&scripts);
    execution.run("t");
    Atom var_value = execution.variables.get(VarY);
    BOOST_CHECK_EQUAL(var_value, Atom("abc"));
    var_value = execution.variables.get(VarZ);
    BOOST_CHECK_EQUAL(var_value, Atom("def"));
}

BOOST_AUTO_TEST_CASE(concat) {
    play_message("DefineScript(t, ["
        "SetVariable(VarX, 2),"
        "SetVariable(VarY, Concat(hello, $VarX))])");

    Execution execution(&scripts);
    execution.run("t");
    std::string var_value = execution.variables.get(VarY);
    BOOST_CHECK_EQUAL(var_value, "hello2");
}

BOOST_AUTO_TEST_CASE(return_value) {
    play_message("DefineScript(u, [Return(\"xyz\")])");
    play_message("DefineScript(t, ["
        "SetVariable(VarX, [Call(u)])])");

    Execution execution(&scripts);
    execution.run("t");
    const std::string& var_value = execution.variables.get(VarX);
    BOOST_CHECK_EQUAL(var_value, "xyz");
}

BOOST_AUTO_TEST_CASE(increment) {
    play_message("DefineScript(t, ["
        "SetVariable(VarX, 5),"
        "Increment(VarX, 3),"
        "SetVariable(VarY, 5),"
        "Increment(VarY)])");

    Execution execution(&scripts);
    execution.run("t");
    int var_value = execution.variables.get(VarX);
    BOOST_CHECK_EQUAL(var_value, 8);
    var_value = execution.variables.get(VarY);
    BOOST_CHECK_EQUAL(var_value, 6);
}

BOOST_AUTO_TEST_CASE(parameter) {
    play_message("DefineScript(u, [VarZ], [Return($VarZ)])");
    play_message("DefineScript(t, ["
        "SetVariable(VarX, 99),"
        "SetVariable(VarY, [Call(u, [42])])])");

    Execution execution(&scripts);
    execution.run("t");
    int var_value = execution.variables.get(VarX);
    BOOST_CHECK_EQUAL(var_value, 99);
    var_value = execution.variables.get(VarY);
    BOOST_CHECK_EQUAL(var_value, 42);
}

BOOST_AUTO_TEST_CASE(while_lt) {
    play_message("DefineScript(t, ["
        "SetVariable(VarX, 2),"
        "While(Lt($VarX, 10), [Increment(VarX, 3)])])");

    Execution execution(&scripts);
    execution.run("t");
    int var_value = execution.variables.get(VarX);
    BOOST_CHECK_EQUAL(var_value, 11);
}

BOOST_AUTO_TEST_SUITE_END()

};
