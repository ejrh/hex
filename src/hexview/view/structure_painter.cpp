#include "common.h"

#include "hexutil/scripting/scripting.h"

#include "hexgame/game/game.h"

#include "hexview/view/view.h"
#include "hexview/view/structure_painter.h"


void StructurePainter::repaint(StructureView& structure_view) {
    Timer paint_time(structure_paint_time);

    Structure& structure = *structure_view.structure;
    Tile& tile = game->level.tiles[structure.position];

    structure_view.paint.clear();
    Script *script = structure_view.view_def->script.get();
    if (!script)
        return;

    // The properties order for a structure paint script is:
    //   - Variables
    //   - Structure view
    //   - Structure view def
    //   - Structure
    //   - Structure type
    //   - Tile
    //   - Tile type
    // Initial variables set:
    //   - terrain_type
    //   - faction_type (if owned)
    //   - selected
    PaintExecution execution(&resources->scripts, resources, &structure_view.paint);
    //execution.add_properties(structure_view.properties);
    //execution.add_properties(structure_view.view_def->properties);
    execution.add_properties(&structure.properties);
    execution.add_properties(&structure.type->properties);
    //execution.add_properties(&tile.properties);
    execution.add_properties(&tile.type->properties);

    Atom terrain_type_atom = AtomRegistry::atom("terrain_type");
    execution.variables.set<std::string>(terrain_type_atom, tile.type->name);

    if (structure.owner) {
        Atom faction_type_atom = AtomRegistry::atom("faction_type");
        execution.variables.set<std::string>(faction_type_atom, structure.owner->type_name);
    }

    Atom selected_atom = AtomRegistry::atom("selected");
    execution.variables.set<bool>(selected_atom, structure_view.selected);

    try {
        execution.run(script);
    } catch (ScriptError& err) {
        BOOST_LOG_TRIVIAL(error) << boost::format("Error in script %s: %s") % script->name % err.what();
        ++structure_paint_error;
    }
    ++structure_paint_counter;
}
