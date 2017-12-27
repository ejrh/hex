#include "common.h"

#include "hexutil/basics/point.h"
#include "hexutil/basics/hexgrid.h"

#include "hexgame/game/game.h"

#include "hexav/resources/paint.h"
#include "hexav/resources/resources.h"

#include "hexview/view/transition_paint.h"
#include "hexview/view/view.h"
#include "hexview/view/view_def.h"


bool TransitionPaintExecution::apply_transition(const std::vector<int>& dirs, const std::vector<int>& frame_nums) {

    for (auto dir_iter = dirs.begin(); dir_iter != dirs.end(); dir_iter++) {
        Point neighbour_pos = get_neighbour(tile_pos, *dir_iter);
        if (!game->level.contains(neighbour_pos)) {
            return false;
        }
        TileViewDef::pointer neighbour_def = view->level_view.tile_views[neighbour_pos].view_def;
        if (!neighbour_def) {
            return false;
        }

        if (!boost::regex_match(neighbour_def->name, pattern_re)) {
            return false;
        }
    }

    if (frame_nums.empty())
        return false;

    int variation = get("tile_variation");
    int r = variation % frame_nums.size();
    paint_frame(frame_nums[r]);

    return true;
}


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


void register_transition_paint_interpreters() {
    InterpreterRegistry::register_interpreter(new TransitionMatchInterpreter());
    InterpreterRegistry::register_interpreter(new TransitionInterpreter());
}
