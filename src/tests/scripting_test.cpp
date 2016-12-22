#include "common.h"

#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"
#include "hexutil/scripting/scripting.h"

#include "hexgame/game/game_messages.h"

#define BOOST_TEST_MODULE ScriptingTest
#include <boost/test/included/unit_test.hpp>

class ScriptLoader: public MessageReceiver {
public:
    ScriptLoader(StrMap<Script> *scripts): scripts(scripts) {
    }

    void receive(Message *msg) {
        switch (msg->type) {
            case DefineScript: {
                auto upd = dynamic_cast<DefineScriptMessage *>(msg);
                define_script(upd->data1, upd->data2);
            } break;
        }
    }

    void define_script(const std::string& name, MessageSequence& sequence) {
        Compiler compiler;
        Script::pointer script = compiler.compile(name, sequence);
        BOOST_LOG_TRIVIAL(info) << "Compiled script: " << name;
        scripts->put_and_warn(name, script);
    }

private:
    StrMap<Script> *scripts;
};

struct Fixture {
    Fixture(): scripts("scripts"), VarX(AtomRegistry::atom("VarX")), VarY(AtomRegistry::atom("VarY")) {
        register_builtin_messages();
    }

    void play_message(const char *input) {
        ScriptLoader loader(&scripts);
        std::istringstream is(input);
        replay_messages(is, loader, "input");
    }

public:
    StrMap<Script> scripts;
    Atom VarX, VarY;
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

BOOST_AUTO_TEST_CASE(include_script) {
    play_message("DefineScript(u, [SetVariable(VarX, \"xyz\")])");
    play_message("DefineScript(t, [SetVariable(VarX, \"abc\"), IncludeScript(u)])");

    Execution execution(&scripts);
    execution.run("t");
    std::string var_value = execution.variables.get(VarX);
    BOOST_CHECK_EQUAL(var_value, "xyz");
}

BOOST_AUTO_TEST_CASE(if_match) {
    play_message("DefineScript(t, ["
        "SetVariable(VarX, 32),"
        "SetVariable(StrVar, \"abc\"),"
        "IfMatch(StrVar, \"a.+\", SetVariable(VarX, 40)),"
        "SetVariable(VarY, 4),"
        "IfMatch(StrVar, \"x.+\", SetVariable(VarY, 7))])");

    Execution execution(&scripts);
    execution.run("t");
    int var_value = execution.variables.get(VarX);
    BOOST_CHECK_EQUAL(var_value, 40);
    var_value = execution.variables.get(VarY);
    BOOST_CHECK_EQUAL(var_value, 4);
}

BOOST_AUTO_TEST_SUITE_END()
