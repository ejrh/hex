#include "common.h"

#include "hexutil/scripting/scripting.h"

#include "hexgame/game/game_messages.h"

#include "hexview/resources/resource_messages.h"
#include "hexview/resources/paint.h"

class PaintFrameInterpreter: public Interpreter {
public:
    PaintFrameInterpreter(): Interpreter("PaintFrame") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        int frame_num = execution->get_argument(instruction, 0);

        PaintExecution* pe = dynamic_cast<PaintExecution *>(execution);
        if (!pe)
            throw ScriptError() << "PaintFrame can only be executed in a PaintExecution";
        Atom paint_library = execution->get(pe->paint_library_atom).get_as_atom();
        int offset_x = execution->get_as<int>(pe->paint_offset_x_atom);
        int offset_y = execution->get_as<int>(pe->paint_offset_y_atom);
        int blend_alpha = execution->get_as<int>(pe->paint_blend_alpha_atom);
        int blend_addition = execution->get_as<int>(pe->paint_blend_addition_atom);
        int frame_offset = execution->get_as<int>(pe->paint_frame_offset_atom);
        pe->paint_frame(paint_library, frame_offset + frame_num, offset_x, offset_y, blend_alpha, blend_addition);

        return 0;
    }
};

class PaintAnimationInterpreter: public Interpreter {
public:
    PaintAnimationInterpreter(): Interpreter("PaintAnimation") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        int frame_time = execution->get_argument(instruction, 0);
        std::vector<int> frame_nums;
        const CompoundTerm *frame_nums_subterm = dynamic_cast<const CompoundTerm *>(execution->get_subterm(instruction, 1));
        for (int i = 0; i < frame_nums_subterm->subterms.size(); i++) {
            int frame_num = execution->get_argument(frame_nums_subterm, i);
            frame_nums.push_back(frame_num);
        }

        PaintExecution* pe = dynamic_cast<PaintExecution *>(execution);
        if (!pe)
            throw ScriptError() << "PaintAnimation can only be executed in a PaintExecution";
        Atom paint_library = execution->get(pe->paint_library_atom);
        int offset_x = execution->get(pe->paint_offset_x_atom);
        int offset_y = execution->get(pe->paint_offset_y_atom);
        int blend_alpha = execution->get(pe->paint_blend_alpha_atom);
        int blend_addition = execution->get(pe->paint_blend_addition_atom);
        int frame_offset = execution->get(pe->paint_frame_offset_atom);
        std::vector<int> adjusted_frame_nums;
        for (auto iter = frame_nums.begin(); iter != frame_nums.end(); iter++) {
            adjusted_frame_nums.push_back(frame_offset + *iter);
        }
        int frame_rate = frame_ms_to_rate(frame_time);
        pe->paint_animation(paint_library, frame_rate, adjusted_frame_nums, offset_x, offset_y, blend_alpha, blend_addition);

        return 0;
    }
};

void register_paint_interpreters() {
    InterpreterRegistry::register_interpreter(new PaintFrameInterpreter());
    InterpreterRegistry::register_interpreter(new PaintAnimationInterpreter());
}
