#ifndef UTILS_H
#define UTILS_H

#include "hexutil/basics/atom.h"
#include "hexutil/basics/point.h"

#include "hexview/view/view.h"


namespace hex {

class FeatureType;
class StructureType;

bool compatible(const Atom tile_type, const FeatureType& feature_type);
bool compatible(const Atom tile_type, const Atom feature_type, const StructureType& structure_type);
std::vector<Point> get_shape_points(Point point, Atom shape);
bool level_contains_points(const Level& level, const std::vector<Point>& points);
bool level_tile_types_match(const Level& level, const std::vector<Point>& points, const std::string& required_pattern);
bool level_feature_types_match(const Level& level, const std::vector<Point>& points, const std::string& required_pattern);
void set_level_feature_type(Level& level, const std::vector<Point>& points, FeatureType::pointer feature_type);

};

#endif

