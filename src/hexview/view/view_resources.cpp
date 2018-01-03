#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/basics/logging.h"

#include "hexview/view/view_resources.h"
#include "hexview/view/view_resource_loader.h"


namespace hex {

ViewResources::ViewResources():
        Resources(),
        tile_view_defs("tile_view_defs"), feature_view_defs("feature_view_defs"),
        unit_view_defs("unit_view_defs"), structure_view_defs("structure_view_defs"),
        faction_view_defs("faction_view_defs") {
}


ViewResources::~ViewResources() { }


void ViewResources::resolve_references() {
    for (auto iter = image_series.begin(); iter != image_series.end(); iter++) {
        resolve_image_series(iter->second);
    }

    for (auto def_iter = faction_view_defs.begin(); def_iter != faction_view_defs.end(); def_iter++) {
        FactionViewDef& def = *def_iter->second;
        resolve_image_series(def.big_flag_images);
        resolve_image_series(def.small_flag_images);
        resolve_image_series(def.shield_images);
    }

    for (auto def_iter = unit_view_defs.begin(); def_iter != unit_view_defs.end(); def_iter++) {
        UnitViewDef& def = *def_iter->second;
        for (auto sound_iter = def.sounds.begin(); sound_iter != def.sounds.end(); sound_iter++) {
            resolve_sound_series(sound_iter->sounds);
        }
    }
}

TileViewDef::pointer ViewResources::create_tile_view(const TileViewDef& data) {
    TileViewDef::pointer def = boost::make_shared<TileViewDef>(data);
    tile_view_defs.put_and_warn(def->name, def);
    return def;
}

FeatureViewDef::pointer ViewResources::create_feature_view(const FeatureViewDef& data) {
    FeatureViewDef::pointer def = boost::make_shared<FeatureViewDef>(data);
    feature_view_defs.put_and_warn(def->name, def);
    return def;
}

StructureViewDef::pointer ViewResources::create_structure_view(const StructureViewDef& data) {
    StructureViewDef::pointer def = boost::make_shared<StructureViewDef>(data);
    structure_view_defs.put_and_warn(def->name, def);
    return def;
}

TileViewDef::pointer ViewResources::get_tile_view_def(const std::string& name) {
    return tile_view_defs.get_and_warn(name);
}

FeatureViewDef::pointer ViewResources::get_feature_view_def(const std::string& name) {
    return feature_view_defs.get_and_warn(name);
}

UnitViewDef::pointer ViewResources::get_unit_view_def(const std::string& name) {
    return unit_view_defs.get_and_warn(name);
}

StructureViewDef::pointer ViewResources::get_structure_view_def(const std::string& name) {
    return structure_view_defs.get_and_warn(name);
}

FactionViewDef::pointer ViewResources::get_faction_view_def(const std::string& name) {
    return faction_view_defs.get_and_warn(name);
}

};
