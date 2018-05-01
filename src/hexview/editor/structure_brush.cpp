#include "common.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"

#include "hexview/editor/brush.h"


namespace hex {

void StructureBrush::paint(const Point point, int radius, Game *game, GameView *view) {
    const Tile& tile = game->level.tiles[point];

    Atom current_structure_type = tile.structure ? tile.structure->type->name : AtomRegistry::get_instance().empty;
    Atom new_structure_type = current_structure_type;

    bool remove_structure = paint_tile(tile, new_structure_type);
    if (remove_structure)
        view->dispatcher->receive(create_message(DestroyStructure, point));

    if (remove_structure || new_structure_type != current_structure_type) {
        //TODO specify the owner somehow in the editor
        int owner_id = structure_type->has_property(Capturable) ? 1 : 0;
        view->dispatcher->receive(create_message(CreateStructure, point, new_structure_type, owner_id));
    }
}

bool StructureBrush::paint_tile(const Tile& tile, Atom& new_structure_type) {
    if (tile.has_property(Immutable)) {
        return false;
    }

    //TODO check whether we can change the feature type to structure?

    if (!compatible(tile.type->name, tile.feature_type->name, *structure_type)) {
        return false;
    }

    new_structure_type = structure_type->name;

    if (tile.structure) {
        return true;
    }

    return false;
}

};
