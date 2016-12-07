#include "common.h"

#include "hexutil/basics/objmap.h"
#include "hexutil/scripting/scripting.h"


void Execution::add_properties(Properties *properties) {
    properties_list.push_back(properties);
}

void Execution::run(Script *script) {
    execute_script(script);
}

void Execution::run(const std::string& script_name) {
    Script *script = scripts->get(script_name).get();
    execute_script(script);
}

void Execution::execute_script(Script *script) {
    execute_sequence(script->instructions);
    return_active = false;
}

void Execution::execute_sequence(InstructionSequence& instructions) {
    for (auto iter = instructions.begin(); iter != instructions.end(); iter++) {
        Instruction *instr = *iter;
        execute_instruction(instr);
        if (return_active)
            break;
    }
}

void Execution::execute_instruction(Instruction *instr) {
    instr->execute(this);
}
