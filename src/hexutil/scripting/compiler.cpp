#include "common.h"

#include "hexutil/scripting/scripting.h"


Compiler::Compiler() {
    register_instruction_compiler(new BuiltinInstructionCompiler);
}

void Compiler::register_instruction_compiler(InstructionCompiler *instruction_compiler) {
    instruction_compilers.push_back(instruction_compiler);
}

Script::pointer Compiler::compile(const std::string& name, MessageSequence& sequence) {
    InstructionSequence instructions = compile_sequence(sequence);
    return boost::make_shared<Script>(name, instructions);
}

InstructionSequence Compiler::compile_sequence(MessageSequence sequence) {
    InstructionSequence instructions;
    for (auto iter = sequence.begin(); iter != sequence.end(); iter++) {
        Instruction *instruction = compile_instruction(iter->get());
        if (instruction) {
            instructions.push_back(instruction);
        }
    }
    return instructions;
}

Instruction *Compiler::compile_instruction(Message *message) {
    for (auto iter = instruction_compilers.rbegin(); iter != instruction_compilers.rend(); iter++) {
        Instruction *instr = (*iter)->compile(message, this);
        if (instr) {
            return instr;
        }
    }

    BOOST_LOG_TRIVIAL(error) << "Unrecognised instruction: " << MessageTypeRegistry::get_message_type_name(message->type);
    return nullptr;
}
