#include "common.h"

#include "hexutil/basics/hexgrid.h"
#include "hexutil/basics/string_vector.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"


namespace hex {

bool compatible(const Atom tile_type, const FeatureType& feature_type) {
    if (feature_type.has_property(RequiredTileType)) {
        std::string required_pattern = feature_type.get_property<std::string>(RequiredTileType);
        boost::regex required_re(required_pattern);
        if (!boost::regex_match(static_cast<const std::string&>(tile_type), required_re))
            return false;
    }
    return true;
}

bool compatible(const Atom tile_type, const Atom feature_type, const StructureType& structure_type) {
    if (structure_type.has_property(RequiredTileType)) {
        std::string required_pattern = structure_type.get_property<std::string>(RequiredTileType);
        boost::regex required_re(required_pattern);
        if (!boost::regex_match(static_cast<const std::string&>(tile_type), required_re))
            return false;
    }
    if (structure_type.has_property(RequiredFeatureType)) {
        std::string required_pattern = structure_type.get_property<std::string>(RequiredFeatureType);
        boost::regex required_re(required_pattern);
        if (!boost::regex_match(static_cast<const std::string&>(feature_type), required_re))
            return false;
    }
    return true;
}

std::vector<Point> get_shape_points(Point point, Atom shape) {
    std::vector<Point> result;

    result.push_back(point);

    if (shape == Singular2x2 || shape == Singular3x3) {
        PointNeighbours neighbour_pos = get_neighbours(point);
        result.push_back(neighbour_pos[4]);
        result.push_back(neighbour_pos[3]);
        result.push_back(neighbour_pos[2]);

        if (shape == Singular3x3) {
            PointNeighbours neighbour_pos_l = get_neighbours(neighbour_pos[4]);
            PointNeighbours neighbour_pos_b = get_neighbours(neighbour_pos[3]);
            PointNeighbours neighbour_pos_r = get_neighbours(neighbour_pos[2]);
            result.push_back(neighbour_pos_l[4]);
            result.push_back(neighbour_pos_b[4]);
            result.push_back(neighbour_pos_b[3]);
            result.push_back(neighbour_pos_b[2]);
            result.push_back(neighbour_pos_r[2]);
        }

        std::sort(result.begin(), result.end());
    }

    return result;
}

bool level_contains_points(const Level& level, const std::vector<Point>& points) {
    for (auto iter = points.begin(); iter != points.end(); iter++) {
        if (!level.contains(*iter))
            return false;
    }

    return true;
}

bool level_tile_types_match(const Level& level, const std::vector<Point>& points, const std::string& required_pattern) {
    boost::regex required_re(required_pattern);

    for (auto iter = points.begin(); iter != points.end(); iter++) {
        Atom type_name = level.tiles[*iter].type->name;
        if (!boost::regex_match(static_cast<const std::string&>(type_name), required_re))
            return false;
    }

    return true;
}

bool level_feature_types_match(const Level& level, const std::vector<Point>& points, const std::string& required_pattern) {
    boost::regex required_re(required_pattern);

    for (auto iter = points.begin(); iter != points.end(); iter++) {
        Atom type_name = level.tiles[*iter].feature_type->name;
        if (!boost::regex_match(static_cast<const std::string&>(type_name), required_re))
            return false;
    }

    return true;
}

void set_level_feature_type(Level& level, const std::vector<Point>& points, FeatureType::pointer feature_type) {
    for (auto iter = points.begin(); iter != points.end(); iter++) {
        level.tiles[*iter].feature_type = feature_type;
    }
}

};
