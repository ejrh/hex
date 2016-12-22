#include "common.h"

#include "hexutil/scripting/scripting.h"

#include "hexgame/game/game_messages.h"

#include "hexview/resources/resource_messages.h"
#include "hexview/resources/paint.h"

class PaintFrameInstruction: public Instruction {
public:
    PaintFrameInstruction(int frame_num):
        frame_num(frame_num) {
    }

    void execute(Execution *execution) {
        PaintExecution* pe = dynamic_cast<PaintExecution *>(execution);
        if (!pe)
            throw ScriptError() << "PaintFrame can only be executed in a PaintExecution";
        Atom paint_library = execution->get(pe->paint_library_atom);
        int offset_x = execution->get(pe->paint_offset_x_atom);
        int offset_y = execution->get(pe->paint_offset_y_atom);
        int blend_addition = execution->get(pe->paint_blend_addition_atom);
        pe->paint_frame(paint_library, frame_num, offset_x, offset_y, blend_addition);
    }

public:
    int frame_num;
};

class PaintAnimationInstruction: public Instruction {
public:
    PaintAnimationInstruction(int frame_rate, std::vector<int>& frame_nums):
        frame_rate(frame_rate), frame_nums(frame_nums) {
    }

    void execute(Execution *execution) {
        PaintExecution* pe = dynamic_cast<PaintExecution *>(execution);
        if (!pe)
            throw ScriptError() << "PaintAnimation can only be executed in a PaintExecution";
        Atom paint_library = execution->get(pe->paint_library_atom);
        int offset_x = execution->get(pe->paint_offset_x_atom);
        int offset_y = execution->get(pe->paint_offset_y_atom);
        int blend_addition = execution->get(pe->paint_blend_addition_atom);
        pe->paint_animation(paint_library, frame_rate, frame_nums, offset_x, offset_y, blend_addition);
    }

public:
    int frame_rate;
    std::vector<int> frame_nums;
};

Instruction *PaintInstructionCompiler::compile(Message *message, Compiler *compiler) {
    switch (message->type) {
        case PaintFrame: {
            auto instr = dynamic_cast<PaintFrameMessage *>(message);
            return new PaintFrameInstruction(instr->data);
        } break;

        case PaintAnimation: {
            auto instr = dynamic_cast<PaintAnimationMessage *>(message);
            return new PaintAnimationInstruction(instr->data1, instr->data2);
        } break;

        default: {
            return nullptr;
        }
    }
}
