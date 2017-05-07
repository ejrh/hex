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

        pe->paint_frame(frame_num);

        return 0;
    }
};

class PaintAnimationInterpreter: public Interpreter {
public:
    PaintAnimationInterpreter(): Interpreter("PaintAnimation") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        int frame_time = execution->get_argument(instruction, 0);
        std::vector<int> frame_nums = execution->get_as_intvector(instruction, 1);

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

class TransitionMatchInterpreter: public Interpreter {
public:
    TransitionMatchInterpreter(): Interpreter("TransitionMatch") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        TransitionPaintExecution* tpe = dynamic_cast<TransitionPaintExecution *>(execution);
        if (!tpe)
            throw ScriptError() << "TransitionMatch can only be executed in a TransitionPaintExecution";

        const std::string& pattern = execution->get_argument(instruction, 0).get_as_str();

        tpe->pattern_re = boost::regex(pattern);

        return 0;
    }
};

class TransitionInterpreter: public Interpreter {
public:
    TransitionInterpreter(): Interpreter("Transition") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        TransitionPaintExecution* tpe = dynamic_cast<TransitionPaintExecution *>(execution);
        if (!tpe)
            throw ScriptError() << "TransitionMatch can only be executed in a TransitionPaintExecution";

        std::vector<int> dir_nums = execution->get_as_intvector(instruction, 0);
        std::vector<int> frame_nums = execution->get_as_intvector(instruction, 1);

        return tpe->apply_transition(dir_nums, frame_nums);
    }
};


void register_paint_interpreters() {
    InterpreterRegistry::register_interpreter(new PaintFrameInterpreter());
    InterpreterRegistry::register_interpreter(new PaintAnimationInterpreter());

    InterpreterRegistry::register_interpreter(new TransitionMatchInterpreter());
    InterpreterRegistry::register_interpreter(new TransitionInterpreter());
}
