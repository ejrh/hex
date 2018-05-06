#include "common.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"

#include "hexview/editor/brush.h"


namespace hex {

void StructureBrush::paint(const Point point, int radius, Game *game, GameView *view) {
    const Tile& tile = game->level.tiles[point];

    Atom current_feature_type = tile.feature_type->name;
    Atom new_feature_type = current_feature_type;

    Atom current_structure_type = tile.structure ? tile.structure->type->name : AtomRegistry::get_instance().empty;
    Atom new_structure_type = current_structure_type;

    bool remove_structure = paint_tile(tile, new_feature_type, new_structure_type, game);
    if (remove_structure)
        view->dispatcher->receive(create_message(DestroyStructure, point));

    if (new_feature_type != current_feature_type) {
        CompressableStringVector type_data;
        CompressableStringVector feature_data;
        type_data.push_back(tile.type->name);
        feature_data.push_back(new_feature_type);
        view->dispatcher->receive(create_message(SetLevelData, point, type_data, feature_data));
    }

    if (remove_structure || new_structure_type != current_structure_type) {
        //TODO specify the owner somehow in the editor
        int owner_id = structure_type->has_property(Capturable) ? 1 : 0;
        view->dispatcher->receive(create_message(CreateStructure, point, new_structure_type, owner_id));
    }
}

bool StructureBrush::can_drag() {
    return false;
}

bool StructureBrush::paint_tile(const Tile& tile, Atom& new_feature_type, Atom& new_structure_type, Game *game) {
    if (tile.has_property(Immutable)) {
        return false;
    }

    if (!compatible(tile.type->name, tile.feature_type->name, *structure_type)) {
        /* If the RequiredFeatureType is a single feature type, and it's compatible with the tile type, then we can
           change the feature type to that one. */
        Atom required_feature_type = structure_type->get_property<Atom>(RequiredFeatureType);
        FeatureType::pointer feature_type = game->feature_types.find(required_feature_type);
        if (feature_type && compatible(tile.type->name, *feature_type))
            new_feature_type = required_feature_type;
        else
            return false;
    }

    new_structure_type = structure_type->name;

    if (tile.structure) {
        return true;
    }

    return false;
}

};
