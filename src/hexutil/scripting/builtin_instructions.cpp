#include "common.h"

#include "hexutil/basics/objmap.h"
#include "hexutil/scripting/scripting.h"


class ListInterpreter: public Interpreter {
public:
    ListInterpreter(): Interpreter("[]") { }

    void execute(const Term *instruction, Execution *execution) const {
        const CompoundTerm *list_term = dynamic_cast<const CompoundTerm *>(instruction);

        for (auto iter = list_term->subterms.begin(); iter != list_term->subterms.end(); iter++) {
            const Term *instr = iter->get();
            execution->execute_instruction(instr);
            if (execution->return_active)
                break;
        }
    }
};


class IncludeScriptInterpreter: public Interpreter {
public:
    IncludeScriptInterpreter(): Interpreter("IncludeScript") { }

    void execute(const Term *instruction, Execution *execution) const {
        const Atom script_name = execution->get_argument(instruction, 0).get_as_atom();
        StrMap<Script>& scripts = *execution->scripts;
        Script *script = scripts.get(script_name).get();
        execution->execute_script(script);
    }
};


class SetVariableInterpreter: public Interpreter {
public:
    SetVariableInterpreter(): Interpreter("SetVariable") { }

    void execute(const Term *instruction, Execution *execution) const {
        const Atom& property = execution->get_argument(instruction, 0);
        const Datum& value = execution->get_argument(instruction, 1);
        execution->variables[property] = value;
    }
};


class IfMatchInterpreter: public Interpreter {
public:
    IfMatchInterpreter(): Interpreter("IfMatch") { }

    void execute(const Term *instruction, Execution *execution) const {
        const std::string& value = execution->get_argument(instruction, 0);
        const std::string& pattern = execution->get_argument(instruction, 1);
        const Term *body = execution->get_subterm(instruction, 2);

        boost::regex pattern_re(pattern);
        if (boost::regex_match(value, pattern_re)) {
            execution->execute_instruction(body);
        }
    }
};


class IfEqInterpreter: public Interpreter {
public:
    IfEqInterpreter(): Interpreter("IfEq") { }

    void execute(const Term *instruction, Execution *execution) const {
        const Datum& value = execution->get_argument(instruction, 0);
        const Datum& value2 = execution->get_argument(instruction, 1);
        const Term *body = execution->get_subterm(instruction, 2);

        if (value == value2) {
            execution->execute_instruction(body);
        }
    }
};


class ReturnInterpreter: public Interpreter {
public:
    ReturnInterpreter(): Interpreter("Return") { }

    void execute(const Term *instruction, Execution *execution) const {
        execution->return_active = true;
    }
};


void register_builtin_interpreters() {
    InterpreterRegistry::register_interpreter(new ListInterpreter());
    InterpreterRegistry::register_interpreter(new IncludeScriptInterpreter());
    InterpreterRegistry::register_interpreter(new SetVariableInterpreter());
    InterpreterRegistry::register_interpreter(new IfMatchInterpreter());
    InterpreterRegistry::register_interpreter(new IfEqInterpreter());
    InterpreterRegistry::register_interpreter(new ReturnInterpreter());
}
