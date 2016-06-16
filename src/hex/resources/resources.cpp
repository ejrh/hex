#include "common.h"

#include "hex/basics/error.h"
#include "hex/resources/resources.h"


void Resources::resolve_references() {
    for (std::map<std::string, ImageSeries>::iterator iter = image_series.begin(); iter != image_series.end(); iter++) {
        resolve_image_series(iter->second);
    }

    for (StrMap<UnitViewDef>::iterator def_iter = unit_view_defs.begin(); def_iter != unit_view_defs.end(); def_iter++) {
        UnitViewDef& def = *def_iter->second;
        for (std::vector<AnimationDef>::iterator anim_iter = def.hold_animations.begin(); anim_iter != def.hold_animations.end(); anim_iter++) {
            resolve_image_series(anim_iter->images);
        }
        for (std::vector<AnimationDef>::iterator anim_iter = def.move_animations.begin(); anim_iter != def.move_animations.end(); anim_iter++) {
            resolve_image_series(anim_iter->images);
        }
        for (std::vector<AnimationDef>::iterator anim_iter = def.attack_animations.begin(); anim_iter != def.attack_animations.end(); anim_iter++) {
            resolve_image_series(anim_iter->images);
        }
        for (std::vector<AnimationDef>::iterator anim_iter = def.recoil_animations.begin(); anim_iter != def.recoil_animations.end(); anim_iter++) {
            resolve_image_series(anim_iter->images);
        }
        for (std::vector<AnimationDef>::iterator anim_iter = def.die_animations.begin(); anim_iter != def.die_animations.end(); anim_iter++) {
            resolve_image_series(anim_iter->images);
        }
    }

    for (StrMap<TileViewDef>::iterator def_iter = tile_view_defs.begin(); def_iter != tile_view_defs.end(); def_iter++) {
        TileViewDef& def = *def_iter->second;
        resolve_image_series(def.animation.images);
        for (std::vector<TransitionDef>::iterator trans_iter = def.transitions.begin(); trans_iter != def.transitions.end(); trans_iter++) {
            resolve_image_series(trans_iter->images);
        }
        resolve_image_series(def.roads);
        for (std::vector<FeatureDef>::iterator feat_iter = def.features.begin(); feat_iter != def.features.end(); feat_iter++) {
            resolve_image_series(feat_iter->images);
        }
    }

    for (StrMap<StructureViewDef>::iterator def_iter = structure_view_defs.begin(); def_iter != structure_view_defs.end(); def_iter++) {
        StructureViewDef& def = *def_iter->second;
        resolve_image_series(def.animation.images);
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
        BOOST_LOG_TRIVIAL(warning) << "No image for reference to: " << image_name;
        return false;
    }
    image_ref.image = images[image_name];
    return true;
}

TileViewDef::pointer Resources::create_tile_view(const TileViewDef& data) {
    TileViewDef::pointer def = boost::make_shared<TileViewDef>(data);
    tile_view_defs.put_and_warn(def->name, def);

    TileViewDef::pointer base = find_base(*def);
    if (base) {
        def->animation = base->animation;
        def->transitions = base->transitions;
    }
    return def;
}

StructureViewDef::pointer Resources::create_structure_view(const StructureViewDef& data) {
    StructureViewDef::pointer def = boost::make_shared<StructureViewDef>(data);
    structure_view_defs.put_and_warn(def->name, def);
    return def;
}

TileViewDef::pointer Resources::get_tile_view_def(const std::string& name) {
    return tile_view_defs.get_and_warn(name);
}

UnitViewDef::pointer Resources::get_unit_view_def(const std::string& name) {
    return unit_view_defs.get_and_warn(name);
}

StructureViewDef::pointer Resources::get_structure_view_def(const std::string& name) {
    return structure_view_defs.get_and_warn(name);
}

FactionViewDef::pointer Resources::get_faction_view_def(const std::string& name) {
    return faction_view_defs.get_and_warn(name);
}

TileViewDef::pointer Resources::find_base(const TileViewDef& def) const {
    for (StrMap<TileViewDef>::const_iterator iter = tile_view_defs.begin(); iter != tile_view_defs.end(); iter++) {
        if (iter->second->name == def.name)
            continue;

        std::pair<std::string::const_iterator,std::string::const_iterator> res = std::mismatch(iter->second->name.begin(), iter->second->name.end(), def.name.begin());
        if (res.first == iter->second->name.end()) {
            return iter->second;
        }
    }
    return TileViewDef::pointer();
}
