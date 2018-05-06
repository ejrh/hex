#include "common.h"

#include "hexutil/scripting/scripting.h"

#include "hexav/resources/resource_messages.h"
#include "hexav/resources/paint.h"


namespace hex {

class FrameWidthInterpreter: public Interpreter {
public:
    FrameWidthInterpreter(): Interpreter("FrameWidth") { }

    Datum execute(const CompoundTerm *instruction, Execution *execution) const {
        int frame_num = execution->get_argument(instruction, 0);

        PaintExecution* pe = dynamic_cast<PaintExecution *>(execution);
        if (!pe)
            throw ScriptError() << "FrameWidth can only be executed in a PaintExecution";

        return pe->frame_width(frame_num);
    }
};

class FrameHeightInterpreter: public Interpreter {
public:
    FrameHeightInterpreter(): Interpreter("FrameHeight") { }

    Datum execute(const CompoundTerm *instruction, Execution *execution) const {
        int frame_num = execution->get_argument(instruction, 0);

        PaintExecution* pe = dynamic_cast<PaintExecution *>(execution);
        if (!pe)
            throw ScriptError() << "FrameHeight can only be executed in a PaintExecution";

        return pe->frame_height(frame_num);
    }
};

class PaintFrameInterpreter: public Interpreter {
public:
    PaintFrameInterpreter(): Interpreter("PaintFrame") { }

    Datum execute(const CompoundTerm *instruction, Execution *execution) const {
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

    Datum execute(const CompoundTerm *instruction, Execution *execution) const {
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

class PaintTextInterpreter: public Interpreter {
    public:
        PaintTextInterpreter(): Interpreter("PaintText") { }

        Datum execute(const CompoundTerm *instruction, Execution *execution) const {
            std::string text = execution->get_argument(instruction, 0);

            PaintExecution* pe = dynamic_cast<PaintExecution *>(execution);
            if (!pe)
                throw ScriptError() << "PaintText can only be executed in a PaintExecution";

            int offset_x = execution->get(pe->paint_offset_x_atom);
            int offset_y = execution->get(pe->paint_offset_y_atom);

            pe->paint_text(text, offset_x, offset_y);

            return 0;
        }
    };


void register_paint_interpreters() {
    InterpreterRegistry::register_interpreter(new FrameWidthInterpreter());
    InterpreterRegistry::register_interpreter(new FrameHeightInterpreter());
    InterpreterRegistry::register_interpreter(new PaintFrameInterpreter());
    InterpreterRegistry::register_interpreter(new PaintAnimationInterpreter());
    InterpreterRegistry::register_interpreter(new PaintTextInterpreter());
}

};
