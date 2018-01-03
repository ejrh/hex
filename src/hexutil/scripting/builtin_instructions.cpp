#include "common.h"

#include "hexutil/basics/objmap.h"
#include "hexutil/scripting/scripting.h"


namespace hex {

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


class CallInterpreter: public Interpreter {
public:
    CallInterpreter(): Interpreter("Call") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        const Atom script_name = execution->get_argument(instruction, 0).get_as_atom();
        StrMap<Script>& scripts = *execution->scripts;
        Script::pointer script = scripts.find(script_name);
        if (!script)
            throw ScriptError() << "Script not found: " << script_name;

        /* Check number of arguments vs parameters. */
        const CompoundTerm *list_term = dynamic_cast<const CompoundTerm *>(instruction);
        const Term *arguments = (list_term->subterms.size() > 1) ? execution->get_subterm(list_term, 1) : nullptr;
        const CompoundTerm *arguments_term = dynamic_cast<const CompoundTerm *>(arguments);
        int num_arguments = arguments_term ? arguments_term->subterms.size() : 0;
        int num_parameters = script->parameters.size();
        if (num_arguments != num_parameters)
            throw ScriptError() << boost::format("Script '%s' expects %d parameters; %d provided") % script_name % num_parameters % num_arguments;

        /* Copy arguments into locals. */
        Properties locals;
        for (int i = 0; i < num_arguments; i++) {
            locals[script->parameters[i]] = execution->get_argument(arguments, i);
        }

        PushLocals push_locals(*execution, locals);
        return execution->execute_script(script.get());
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

        const CompoundTerm *list_term = dynamic_cast<const CompoundTerm *>(instruction);
        const Term *else_body = (list_term->subterms.size() >= 3) ? execution->get_subterm(list_term, 2) : nullptr;

        if (condition) {
            return execution->execute_instruction(body);
        } else if (else_body) {
            return execution->execute_instruction(else_body);
        }

        return 0;
    }
};


class WhileInterpreter: public Interpreter {
public:
    WhileInterpreter(): Interpreter("While") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        const Term *body = execution->get_subterm(instruction, 1);

        Datum result = 0;
        while (execution->get_argument(instruction, 0).get_as_int()) {
            result = execution->execute_instruction(body);
        }

        return result;
    }
};


class MatchInterpreter: public Interpreter {
public:
    MatchInterpreter(): Interpreter("Match") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        const std::string& value = execution->get_argument(instruction, 0).get_as_str();
        const std::string& pattern = execution->get_argument(instruction, 1).get_as_str();

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


class GtInterpreter: public Interpreter {
public:
    GtInterpreter(): Interpreter("Gt") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        int value = execution->get_argument(instruction, 0).get_as_int();
        int value2 = execution->get_argument(instruction, 1).get_as_int();

        if (value > value2) {
            return 1;
        }

        return 0;
    }
};


class LtInterpreter: public Interpreter {
public:
    LtInterpreter(): Interpreter("Lt") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        int value = execution->get_argument(instruction, 0).get_as_int();
        int value2 = execution->get_argument(instruction, 1).get_as_int();

        if (value < value2) {
            return 1;
        }

        return 0;
    }
};


class AndInterpreter: public Interpreter {
public:
    AndInterpreter(): Interpreter("And") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        const CompoundTerm *list_term = dynamic_cast<const CompoundTerm *>(instruction);

        for (unsigned int i = 0; i < list_term->subterms.size(); i++) {
            int condition = execution->get_argument(instruction, i).get_as_int();
            if (!condition) {
                return 0;
            }
        }

        return 1;
    }
};


class OrInterpreter: public Interpreter {
public:
    OrInterpreter(): Interpreter("Or") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        const CompoundTerm *list_term = dynamic_cast<const CompoundTerm *>(instruction);

        for (unsigned int i = 0; i < list_term->subterms.size(); i++) {
            int condition = execution->get_argument(instruction, i).get_as_int();
            if (condition) {
                return 1;
            }
        }

        return 0;
    }
};


class NotInterpreter: public Interpreter {
public:
    NotInterpreter(): Interpreter("Not") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        int condition = execution->get_argument(instruction, 0).get_as_int();

        return !condition;
    }
};


class AddInterpreter: public Interpreter {
public:
    AddInterpreter(): Interpreter("Add") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        int value = execution->get_argument(instruction, 0).get_as_int();
        int value2 = execution->get_argument(instruction, 1).get_as_int();

        return value + value2;
    }
};


class SubInterpreter: public Interpreter {
public:
    SubInterpreter(): Interpreter("Sub") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        int value = execution->get_argument(instruction, 0).get_as_int();
        int value2 = execution->get_argument(instruction, 1).get_as_int();

        return value - value2;
    }
};


class DivInterpreter: public Interpreter {
public:
    DivInterpreter(): Interpreter("Div") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        int value = execution->get_argument(instruction, 0).get_as_int();
        int value2 = execution->get_argument(instruction, 1).get_as_int();

        return value / value2;
    }
};


class ConcatInterpreter: public Interpreter {
public:
    ConcatInterpreter(): Interpreter("Concat") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        const CompoundTerm *list_term = dynamic_cast<const CompoundTerm *>(instruction);
        std::ostringstream result;
        for (unsigned int i = 0; i < list_term->subterms.size(); i++) {
            result << execution->get_argument(list_term, i).get_as_str();
        }

        return result.str();
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


class MixInterpreter: public Interpreter {
public:
    MixInterpreter(): Interpreter("Mix") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        const int seed1 = execution->get_argument(instruction, 0).get_as_int();
        const int seed2 = execution->get_argument(instruction, 1).get_as_int();

        std::hash<std::pair<int, int> > hash;
        return hash(std::pair<int, int>(seed1, seed2));
    }
};


class LogInterpreter: public Interpreter {
public:
    LogInterpreter(): Interpreter("Log") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        const CompoundTerm *list_term = dynamic_cast<const CompoundTerm *>(instruction);
        std::ostringstream message;
        bool first = true;
        for (unsigned int i = 0; i < list_term->subterms.size(); i++) {
            if (!first)
                message << ' ';
            message << execution->get_argument(list_term, i).get_as_str();
            first = false;
        }

        BOOST_LOG_TRIVIAL(info) << boost::format("Script line %d: ") % instruction->line_no <<  message.str();
        return 1;
    }
};


class IncrementInterpreter: public Interpreter {
public:
    IncrementInterpreter(): Interpreter("Increment") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        const CompoundTerm *list_term = dynamic_cast<const CompoundTerm *>(instruction);
        const Atom& property = execution->get_argument(instruction, 0);
        int value = execution->variables[property].get_as_int();

        if (list_term->subterms.size() == 1) {
            value += 1;
        } else {
            for (unsigned int i = 1; i < list_term->subterms.size(); i++)
                value += execution->get_argument(instruction, i).get_as_int();
        }

        execution->variables[property] = value;
        return value;
    }
};


void register_builtin_interpreters() {
    InterpreterRegistry::register_interpreter(new ListInterpreter());
    InterpreterRegistry::register_interpreter(new CallInterpreter());
    InterpreterRegistry::register_interpreter(new SetVariableInterpreter());
    InterpreterRegistry::register_interpreter(new IfInterpreter());
    InterpreterRegistry::register_interpreter(new WhileInterpreter());
    InterpreterRegistry::register_interpreter(new MatchInterpreter());
    InterpreterRegistry::register_interpreter(new EqInterpreter());
    InterpreterRegistry::register_interpreter(new GtInterpreter());
    InterpreterRegistry::register_interpreter(new LtInterpreter());
    InterpreterRegistry::register_interpreter(new AndInterpreter());
    InterpreterRegistry::register_interpreter(new OrInterpreter());
    InterpreterRegistry::register_interpreter(new NotInterpreter());
    InterpreterRegistry::register_interpreter(new AddInterpreter());
    InterpreterRegistry::register_interpreter(new SubInterpreter());
    InterpreterRegistry::register_interpreter(new DivInterpreter());
    InterpreterRegistry::register_interpreter(new ConcatInterpreter());
    InterpreterRegistry::register_interpreter(new ReturnInterpreter());
    InterpreterRegistry::register_interpreter(new ChooseInterpreter());
    InterpreterRegistry::register_interpreter(new MixInterpreter());
    InterpreterRegistry::register_interpreter(new LogInterpreter());
    InterpreterRegistry::register_interpreter(new IncrementInterpreter());
}

};
