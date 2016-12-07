#include "common.h"

#include "hexutil/basics/objmap.h"
#include "hexutil/scripting/scripting.h"


class IncludeScriptInstruction: public Instruction {
public:
    IncludeScriptInstruction(const std::string& script_name):
        script_name(script_name) { }

    void execute(Execution *execution) {
        StrMap<Script>& scripts = *execution->scripts;
        Script *script = scripts.get(script_name).get();
        execution->execute_script(script);
    }

public:
    std::string script_name;
};


class SetVariableInstruction: public Instruction {
public:
    SetVariableInstruction(const Atom property, const Datum value):
        property(property), value(value) { }

    void execute(Execution *execution) {
        execution->variables[property] = value;
    }

public:
    Atom property;
    Datum value;
};


class IfMatchInstruction: public Instruction {
public:
    IfMatchInstruction(const Atom property, const std::string& pattern, const InstructionSequence& sequence):
        property(property), pattern(pattern), sequence(sequence) { }

    void execute(Execution *execution) {
        std::string var_value = execution->get<std::string>(property);
        boost::regex pattern_re(pattern);
        if (boost::regex_match(var_value, pattern_re)) {
            execution->execute_sequence(sequence);
        }
    }

public:
    Atom property;
    std::string pattern;
    InstructionSequence sequence;
};


class ReturnInstruction: public Instruction {
public:
    ReturnInstruction() { }

    void execute(Execution *execution) {
        execution->return_active = true;
    }
};


Instruction *BuiltinInstructionCompiler::compile(Message *message, Compiler *compiler) {
    switch (message->type) {
        case IncludeScript: {
            auto instr = dynamic_cast<IncludeScriptMessage *>(message);
            return new IncludeScriptInstruction(instr->data);
        } break;

        case SetVariable: {
            auto instr = dynamic_cast<SetVariableMessage *>(message);
            return new SetVariableInstruction(instr->data1, instr->data2);
        } break;

        case IfMatch: {
            auto instr = dynamic_cast<IfMatchMessage *>(message);
            return new IfMatchInstruction(instr->data1, instr->data2, compiler->compile_sequence(instr->data3));
        } break;

        case Return: {
            return new ReturnInstruction();
        } break;

        default: {
            return nullptr;
        }
    }
}
