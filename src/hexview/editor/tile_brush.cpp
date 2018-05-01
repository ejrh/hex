#include "common.h"

#include "hexutil/basics/hexgrid.h"
#include "hexutil/basics/string_vector.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"
#include "hexgame/game/generation/utils.h"

#include "hexview/editor/brush.h"


namespace hex {

void TileTypeBrush::paint(const Point point, int radius, Game *game, GameView *view) {
    if (!tile_type || tile_type->has_property(Immutable))
        return;

    std::vector<int> scanlines = get_circle_scanlines(point, radius);
    for (unsigned int i = 0; i < scanlines.size(); i++) {
        int row = point.y - radius + i;
        if (row < 0 || row >= game->level.height)
            continue;
        int x1 = point.x - scanlines[i];
        int x2 = point.x + scanlines[i];
        if (x1 < 0)
            x1 = 0;
        if (x2 >= game->level.width)
            x2 = game->level.width - 1;

        CompressableStringVector type_data;
        CompressableStringVector feature_data;
        bool changed_tiles = false;
        for (int i = x1; i <= x2; i++) {
            Point tile_pos = Point(i, row);
            const Tile & tile = game->level.tiles[tile_pos];

            Atom new_tile_type = tile.type->name;
            Atom new_feature_type = tile.feature_type->name;

            bool remove_structure = paint_tile(tile, new_tile_type, new_feature_type);
            if (remove_structure)
                view->dispatcher->receive(create_message(DestroyStructure, tile_pos));

            type_data.push_back(new_tile_type);
            feature_data.push_back(new_feature_type);

            if (remove_structure || new_tile_type != tile.type->name || new_feature_type != tile.feature_type->name)
                changed_tiles = true;
        }

        if (changed_tiles)
            view->dispatcher->receive(create_message(SetLevelData, Point(x1, row), type_data, feature_data));
    }
}

bool TileTypeBrush::can_drag() {
    return true;
}


bool TileTypeBrush::paint_tile(const Tile & tile, Atom & new_tile_type, Atom & new_feature_type) {
    if (tile.has_property(Immutable)) {
        return false;
    }

    new_tile_type = tile_type->name;
    if (!compatible(new_tile_type, *tile.feature_type)) {
        std::string default_feature_type = tile_type->get_property<std::string>(DefaultFeatureType);
        new_feature_type = default_feature_type;
    }

    if (tile.structure && !compatible(new_tile_type, new_feature_type, *tile.structure->type)) {
        return true;
    }

    return false;
}


void FeatureTypeBrush::paint(const Point point, int radius, Game *game, GameView *view) {
    if (!feature_type)
        return;

    if (!can_drag()) {
        paint_singular(point, game, view);
        return;
    } else if (feature_type->get_property<Atom>(Shape) == Flowing) {
        radius = 0;
    }

    std::vector<int> scanlines = get_circle_scanlines(point, radius);
    for (unsigned int i = 0; i < scanlines.size(); i++) {
        int row = point.y - radius + i;
        if (row < 0 || row >= game->level.height)
            continue;
        int x1 = point.x - scanlines[i];
        int x2 = point.x + scanlines[i];
        if (x1 < 0)
            x1 = 0;
        if (x2 >= game->level.width)
            x2 = game->level.width - 1;

        CompressableStringVector type_data;
        CompressableStringVector feature_data;
        bool changed_tiles = false;
        for (int i = x1; i <= x2; i++) {
            Point tile_pos = Point(i,row);
            const Tile& tile = game->level.tiles[tile_pos];

            Atom new_feature_type = tile.feature_type->name;

            bool remove_structure = paint_tile(tile, new_feature_type);
            if (remove_structure)
                view->dispatcher->receive(create_message(DestroyStructure, tile_pos));

            type_data.push_back(tile.type->name);
            feature_data.push_back(new_feature_type);

            if (remove_structure || new_feature_type != tile.feature_type->name)
                changed_tiles = true;
        }

        if (changed_tiles)
            view->dispatcher->receive(create_message(SetLevelData, Point(x1,row), type_data, feature_data));
    }
}

bool FeatureTypeBrush::can_drag() {
    Atom shape = feature_type->get_property<Atom>(Shape);

    //TODO don't fall back to 0; add get-with-default for properties, and add a proper default Shape
    return shape == "0" || shape == Flowing;
}

void FeatureTypeBrush::paint_singular(const Point point, Game *game, GameView *view) {
    std::vector<Point> covered_points = get_shape_points(point, feature_type->get_property<Atom>(Shape));

    if (!level_contains_points(game->level, covered_points))
        return;

    std::string required_pattern = feature_type->get_property<std::string>(RequiredTileType);
    if (!level_tile_types_match(game->level, covered_points, required_pattern))
        return;

    //TODO add some kind of iterator for turning a vector of Points into scanlines
    //TODO make sure there aren't gaps! (depends on the shapes we handle)
    CompressableStringVector type_data;
    CompressableStringVector feature_data;
    bool changed_tiles = false;
    Point start(0, -1);
    for (auto iter = covered_points.begin(); iter != covered_points.end(); iter++) {
        const Tile& tile = game->level.tiles[*iter];
        if (iter->y > start.y) {
            if (changed_tiles)
                view->dispatcher->receive(create_message(SetLevelData, start, type_data, feature_data));
            type_data.clear();
            feature_data.clear();
            changed_tiles = false;
            start = *iter;
        }

        Atom new_feature_type = tile.feature_type->name;

        bool remove_structure = paint_tile(tile, new_feature_type);
        if (remove_structure)
            view->dispatcher->receive(create_message(DestroyStructure, *iter));

        new_feature_type = (*iter == point) ? feature_type->name : feature_type->get_property<Atom>(CoveredFeatureType);

        type_data.push_back(tile.type->name);
        feature_data.push_back(new_feature_type);

        if (remove_structure || new_feature_type != tile.feature_type->name)
            changed_tiles = true;
    }

    if (changed_tiles)
        view->dispatcher->receive(create_message(SetLevelData, start, type_data, feature_data));
}

bool FeatureTypeBrush::paint_tile(const Tile& tile, Atom& new_feature_type) {
    if (tile.has_property(Immutable)) {
        return false;
    }

    if (!compatible(tile.type->name, *feature_type)) {
        return false;
    }

    new_feature_type = feature_type->name;

    if (tile.structure && !compatible(tile.type->name, new_feature_type, *tile.structure->type)) {
        return true;
    }

    return false;
}

};
