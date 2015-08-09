#include "common.h"

#include "hex/basics/error.h"
#include "hex/resources/resources.h"


void Resources::resolve_references() {
    for (std::map<std::string, ImageSeries>::iterator iter = image_series.begin(); iter != image_series.end(); iter++) {
        resolve_image_series(iter->second);
    }

    for (UnitViewDefMap::iterator def_iter = unit_view_defs.begin(); def_iter != unit_view_defs.end(); def_iter++) {
        UnitViewDef *def = def_iter->second;
        for (std::vector<AnimationDef>::iterator anim_iter = def->hold_animations.begin(); anim_iter != def->hold_animations.end(); anim_iter++) {
            resolve_image_series(anim_iter->images);
        }
        for (std::vector<AnimationDef>::iterator anim_iter = def->move_animations.begin(); anim_iter != def->move_animations.end(); anim_iter++) {
            resolve_image_series(anim_iter->images);
        }
    }

    for (TileViewDefMap::iterator def_iter = tile_view_defs.begin(); def_iter != tile_view_defs.end(); def_iter++) {
        TileViewDef *def = def_iter->second;
        resolve_image_series(def->animation.images);
        for (std::vector<TransitionDef>::iterator trans_iter = def->transitions.begin(); trans_iter != def->transitions.end(); trans_iter++) {
            resolve_image_series(trans_iter->images);
        }
        resolve_image_series(def->roads);
        for (std::vector<FeatureDef>::iterator feat_iter = def->features.begin(); feat_iter != def->features.end(); feat_iter++) {
            resolve_image_series(feat_iter->images);
        }
    }

    for (StructureViewDefMap::iterator def_iter = structure_view_defs.begin(); def_iter != structure_view_defs.end(); def_iter++) {
        StructureViewDef *def = def_iter->second;
        resolve_image_series(def->animation.images);
    }
}

void Resources::resolve_image_series(std::vector<ImageRef>& image_series) {
    for (std::vector<ImageRef>::iterator ref_iter = image_series.begin(); ref_iter != image_series.end(); ref_iter++) {
        resolve_image_ref(*ref_iter);
    }
}

bool Resources::resolve_image_ref(ImageRef& image_ref) {
    std::string image_name = image_ref.name;
    if (images.find(image_name) == images.end()) {
        warn("No image for reference to: %s", image_name.c_str());
        return false;
    }
    image_ref.image = images[image_name];
    return true;
}

TileViewDef *Resources::create_tile_view(const TileViewDef& data) {
    TileViewDef *def = new TileViewDef(data);
    tile_view_defs[def->name] = def;

    TileViewDef *base = find_base(*def);
    if (base != NULL) {
        def->animation = base->animation;
        def->transitions = base->transitions;
    }
    return def;
}

StructureViewDef *Resources::create_structure_view(const StructureViewDef& data) {
    StructureViewDef *def = new StructureViewDef(data);
    structure_view_defs[def->name] = def;
    return def;
}

TileViewDef *Resources::get_tile_view_def(const std::string& name) const {
    TileViewDefMap::const_iterator iter = tile_view_defs.find(name);
    return (iter != tile_view_defs.end()) ? iter->second : NULL;
}

UnitViewDef *Resources::get_unit_view_def(const std::string& name) {
    UnitViewDefMap::const_iterator iter = unit_view_defs.find(name);
    if (iter != unit_view_defs.end())
        return iter->second;

    warn("No unit view def for: %s", name.c_str());
    UnitViewDef *view_def = new UnitViewDef(name);
    unit_view_defs[name] = view_def;
    return view_def;
}

StructureViewDef *Resources::get_structure_view_def(const std::string& name) const {
    StructureViewDefMap::const_iterator iter = structure_view_defs.find(name);
    if (iter != structure_view_defs.end())
        return iter->second;

    return NULL;
}

FactionViewDef *Resources::get_faction_view_def(const std::string& name) {
    FactionViewDefMap::const_iterator iter = faction_view_defs.find(name);
    if (iter != faction_view_defs.end())
        return iter->second;

    return NULL;
}

TileViewDef *Resources::find_base(const TileViewDef& def) const {
    for (TileViewDefMap::const_iterator iter = tile_view_defs.begin(); iter != tile_view_defs.end(); iter++) {
        if (iter->second->name == def.name)
            continue;

        std::pair<std::string::const_iterator,std::string::const_iterator> res = std::mismatch(iter->second->name.begin(), iter->second->name.end(), def.name.begin());
        if (res.first == iter->second->name.end()) {
            return iter->second;
        }
    }
    return NULL;
}
