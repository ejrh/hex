#include "common.h"

#include "hexav/graphics/graphics.h"

#include "hexav/resources/resources.h"
#include "hexav/ui/ui.h"
#include "hexav/ui/window_painter.h"


namespace hex {

class WindowExecution: public PaintExecution {
public:
    WindowExecution(AtomMap<Script> *scripts, Resources *resources, Paint *paint, UiWindow *window):
            PaintExecution(scripts, resources, paint),
            window(window) {
    }

    void run(Script *script) {
        PaintExecution::run(script);
    }

public:
    UiWindow *window;
};



WindowPainter::WindowPainter(Resources *resources):
        resources(resources),
        window_paint_counter("paint.window"),
        window_paint_time("paint.window.time"),
        window_paint_error("paint.window.error") {
}

void WindowPainter::repaint_window(UiWindow *window) {
    Timer paint_time(window_paint_time);

    window->paint.clear();
    WindowExecution execution(&resources->scripts, resources, &window->paint, window);

    execution.variables["window_name"] = window->name;
    execution.variables["window_width"] = window->width;
    execution.variables["window_height"] = window->height;
    execution.variables["window_has_focus"] = (window->flags & WindowHasFocus) != 0;
    execution.variables["window_has_dragging"] = (window->flags & WindowHasDragging) != 0;
    execution.variables["window_is_selected"] = (window->flags & WindowIsSelected) != 0;

    window->initialise_paint(&execution);

    try {
        execution.run(window->paint_script.get());
    } catch (ScriptError& err) {
        BOOST_LOG_TRIVIAL(error) << boost::format("Error in script %s: %s") % window->paint_script->name % err.what();
        ++window_paint_error;
    }

    ++window_paint_counter;
}


class PositionControlInterpreter: public Interpreter {
public:
    PositionControlInterpreter(): Interpreter("PositionControl") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        Atom control_name = execution->get_argument(instruction, 0).get_as_atom();
        int x = execution->get_argument(instruction, 1).get_as_int();
        int y = execution->get_argument(instruction, 2).get_as_int();
        int w = execution->get_argument(instruction, 3).get_as_int();
        int h = execution->get_argument(instruction, 4).get_as_int();
        Atom control_script_name = execution->get_argument(instruction, 5).get_as_atom();

        WindowExecution* swe = dynamic_cast<WindowExecution *>(execution);
        if (!swe)
            throw ScriptError() << "PositionControl can only be executed in a WindowExecution";

        UiWindow *control = swe->window->get_control(control_name);
        if (!control) {
            BOOST_LOG_TRIVIAL(error) << boost::format("Window '%s' has no such child: '%s'") % swe->window->name % control_name;
            return 0;
        }

        control->x = x;
        control->y = y;
        control->width = w;
        control->height = h;
        control->set_paint_script(*swe->scripts, control_script_name);

        return 1;
    }
};


class HideControlInterpreter: public Interpreter {
public:
    HideControlInterpreter(): Interpreter("HideControl") { }

    Datum execute(const Term *instruction, Execution *execution) const {
        Atom control_name = execution->get_argument(instruction, 0);

        WindowExecution* swe = dynamic_cast<WindowExecution *>(execution);
        if (!swe)
            throw ScriptError() << "HideControl can only be executed in a WindowExecution";

        UiWindow *control = swe->window->get_control(control_name);
        if (!control) {
            BOOST_LOG_TRIVIAL(error) << boost::format("Window '%s' has no such child: '%s'") % swe->window->name % control_name;
            return 0;
        }

        control->clear_flag(WindowIsVisible);

        return 1;
    }
};


void register_window_interpreters() {
    InterpreterRegistry::register_interpreter(new PositionControlInterpreter());
    InterpreterRegistry::register_interpreter(new HideControlInterpreter());
}

};
