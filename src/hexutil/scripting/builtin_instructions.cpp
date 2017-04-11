#include "common.h"

#include "hexutil/basics/objmap.h"
#include "hexutil/scripting/scripting.h"


class ListInterpreter: public Interpreter {
public:
    ListInterpreter(): Interpreter("[]") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        const CompoundTerm *list_term = dynamic_cast<const CompoundTerm *>(instruction);

        Datum rv = 0;
        for (auto iter = list_term->subterms.begin(); iter != list_term->subterms.end(); iter++) {
            const Term *instr = iter->get();
            rv = execution->execute_instruction(instr);
            if (execution->return_active)
                break;
        }

        return rv;
    }
};


class IncludeScriptInterpreter: public Interpreter {
public:
    IncludeScriptInterpreter(): Interpreter("IncludeScript") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        const Atom script_name = execution->get_argument(instruction, 0).get_as_atom();
        StrMap<Script>& scripts = *execution->scripts;
        Script *script = scripts.get(script_name).get();
        return execution->execute_script(script);
    }
};


class SetVariableInterpreter: public Interpreter {
public:
    SetVariableInterpreter(): Interpreter("SetVariable") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        const Atom& property = execution->get_argument(instruction, 0);
        const Datum& value = execution->get_argument(instruction, 1);
        execution->variables[property] = value;
        return value;
    }
};


class IfInterpreter: public Interpreter {
public:
    IfInterpreter(): Interpreter("If") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        int condition = execution->get_argument(instruction, 0).get_as_int();
        const Term *body = execution->get_subterm(instruction, 1);

        if (condition) {
            return execution->execute_instruction(body);
        }

        return 0;
    }
};


class MatchInterpreter: public Interpreter {
public:
    MatchInterpreter(): Interpreter("Match") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        const std::string& value = execution->get_argument(instruction, 0);
        const std::string& pattern = execution->get_argument(instruction, 1);

        boost::regex pattern_re(pattern);
        if (boost::regex_match(value, pattern_re)) {
            return 1;
        }

        return 0;
    }
};


class EqInterpreter: public Interpreter {
public:
    EqInterpreter(): Interpreter("Eq") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        const Datum& value = execution->get_argument(instruction, 0);
        const Datum& value2 = execution->get_argument(instruction, 1);

        if (value == value2) {
            return 1;
        }

        return 0;
    }
};


class ReturnInterpreter: public Interpreter {
public:
    ReturnInterpreter(): Interpreter("Return") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        execution->return_active = true;

        const CompoundTerm *list_term = dynamic_cast<const CompoundTerm *>(instruction);
        if (list_term->subterms.size() >= 1) {
            return execution->get_argument(list_term, 0);
        } else {
            return 0;
        }
    }
};


class ChooseInterpreter: public Interpreter {
public:
    ChooseInterpreter(): Interpreter("Choose") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        const int seed = execution->get_argument(instruction, 0).get_as_int();
        const Term *choices = execution->get_subterm(instruction, 1);
        const CompoundTerm *list_term = dynamic_cast<const CompoundTerm *>(choices);
        return execution->get_argument(choices, seed % list_term->subterms.size());
    }
};


void register_builtin_interpreters() {
    InterpreterRegistry::register_interpreter(new ListInterpreter());
    InterpreterRegistry::register_interpreter(new IncludeScriptInterpreter());
    InterpreterRegistry::register_interpreter(new SetVariableInterpreter());
    InterpreterRegistry::register_interpreter(new IfInterpreter());
    InterpreterRegistry::register_interpreter(new MatchInterpreter());
    InterpreterRegistry::register_interpreter(new EqInterpreter());
    InterpreterRegistry::register_interpreter(new ReturnInterpreter());
    InterpreterRegistry::register_interpreter(new ChooseInterpreter());
}
