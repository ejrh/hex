#include "common.h"

#include "hexutil/basics/hexgrid.h"
#include "hexutil/basics/string_vector.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"

#include "hexview/editor/brush.h"


static bool compatible(const std::string& tile_type, const FeatureType& feature_type) {
    if (feature_type.has_property(RequiredTileType)) {
        std::string required_pattern = feature_type.get_property<std::string>(RequiredTileType);
        boost::regex required_re(required_pattern);
        if (!boost::regex_match(tile_type, required_re))
            return false;
    }
    return true;
}

static bool compatible(const std::string& tile_type, const std::string& feature_type, const StructureType& structure_type) {
    if (structure_type.has_property(RequiredTileType)) {
        std::string required_pattern = structure_type.get_property<std::string>(RequiredTileType);
        boost::regex required_re(required_pattern);
        if (!boost::regex_match(tile_type, required_re))
            return false;
    }
    if (structure_type.has_property(RequiredFeatureType)) {
        std::string required_pattern = structure_type.get_property<std::string>(RequiredFeatureType);
        boost::regex required_re(required_pattern);
        if (!boost::regex_match(feature_type, required_re))
            return false;
    }
    return true;
}


Brush::Brush(Game *game, GameView *view):
     game(game), view(view),
     paint_radius(1) {
}

void Brush::set_type(const std::string& type_name) {
    tile_type = game->tile_types.get(type_name);
}

void Brush::next_type() {
    std::map<std::string,TileType::pointer>& map = game->tile_types;
    if (!tile_type) {
        auto first = map.begin();
        if (first == map.end())
            return;
        tile_type = first->second;
    }

    auto iter = map.find(tile_type->name);
    iter++;
    if (iter == map.end()) {
        iter = map.begin();
    }
    tile_type = iter->second;
    BOOST_LOG_TRIVIAL(info) << "Brush is now: " << tile_type->name;
}

void Brush::paint_tiles(const Point point) {
    if (!tile_type || tile_type->has_property(Immutable))
        return;

    std::vector<int> scanlines;
    get_circle(point, paint_radius, scanlines);
    for (int i = 0; i < scanlines.size(); i++) {
        int row = point.y - paint_radius + i;
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

            std::string new_tile_type = tile.type->name;
            std::string new_feature_type = tile.feature_type->name;

            bool remove_structure = paint_tile(tile, new_tile_type, new_feature_type);
            if (remove_structure)
                view->dispatcher->receive(create_message(DestroyStructure, tile_pos));

            type_data.push_back(new_tile_type);
            feature_data.push_back(new_feature_type);

            if (remove_structure || new_tile_type != tile.type->name || new_feature_type != tile.feature_type->name)
                changed_tiles = true;
        }

        if (changed_tiles)
            view->dispatcher->receive(create_message(SetLevelData, Point(x1,row), type_data, feature_data));
    }
}

bool Brush::paint_tile(const Tile& tile, std::string& new_tile_type, std::string& new_feature_type) {
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
