#include "common.h"

#include "hexutil/basics/hexgrid.h"
#include "hexutil/basics/string_vector.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"

#include "hexview/editor/brush.h"


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

};
