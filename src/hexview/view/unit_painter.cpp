#include "common.h"

#include "hexutil/scripting/scripting.h"

#include "hexgame/game/game.h"

#include "hexview/view/view.h"
#include "hexview/view/unit_painter.h"


namespace hex {

void UnitPainter::repaint(UnitView& unit_view, Unit& unit) {
    Timer paint_time(unit_paint_time);

    unit_view.paint.clear();
    if (!unit_view.view_def)
        return;

    Script *script = unit_view.view_def->script.get();
    if (!script)
        return;

    // The properties order for a unit paint script is:
    //   - Variables
    //   - Unit view
    //   - Unit view def
    //   - Unit
    //   - Unit type
    //   - Tile
    //   - Tile type
    // Initial variables set:
    //   - terrain_type
    //   - faction_type (if owned)
    //   - selected
    PaintExecution execution(&resources->scripts, resources, &unit_view.paint);
    //execution.add_properties(&unit_view.properties);
    //execution.add_properties(&unit_view.view_def->properties);
    execution.add_properties(&unit.properties);
    if (unit.type)
        execution.add_properties(&unit.type->properties);
    //execution.add_properties(&tile.properties);
    //execution.add_properties(&tile.type->properties);

    //Atom terrain_type_atom = AtomRegistry::atom("terrain_type");
    //execution.variables.set<std::string>(terrain_type_atom, tile.type->name);

    /*if (unit.owner) {
        Atom faction_type_atom = AtomRegistry::atom("faction_type");
        execution.variables.set<std::string>(faction_type_atom, unit.owner->type_name);
    }*/

    Atom selected_atom = AtomRegistry::atom("selected");
    execution.variables.set<bool>(selected_atom, unit_view.selected);

    Atom shadow_atom = AtomRegistry::atom("shadow");
    execution.variables.set<bool>(shadow_atom, unit_view.shadow);

    Atom unit_facing_atom = AtomRegistry::atom("unit_facing");
    execution.variables.set<int>(unit_facing_atom, unit_view.facing);

    Atom unit_posture_atom = AtomRegistry::atom("unit_posture");
    Atom posture_atoms[] = { AtomRegistry::atom("holding"), AtomRegistry::atom("moving"), AtomRegistry::atom("attacking"), AtomRegistry::atom("recoiling"), AtomRegistry::atom("dying") };
    execution.variables.set<Atom>(unit_posture_atom, posture_atoms[unit_view.posture]);

    Atom unit_variation_atom = AtomRegistry::atom("unit_variation");
    execution.variables.set<int>(unit_variation_atom, unit_view.variation);

    try {
        execution.run(script);
    } catch (ScriptError& err) {
        BOOST_LOG_TRIVIAL(error) << boost::format("Error in script %s: %s") % script->name % err.what();
        ++script_error_counter;
    }

    ++unit_paint_counter;
}

void UnitPainter::repaint(UnitStackView& unit_stack_view, UnitStack& unit_stack) {
    Unit& unit = *unit_stack.units[unit_stack_view.representative];
    repaint(unit_stack_view, unit);
}

};
