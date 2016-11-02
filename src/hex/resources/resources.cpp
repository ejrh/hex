#include "common.h"

#include "hex/basics/error.h"
#include "hex/resources/resources.h"


void Resources::resolve_references() {
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
        for (auto anim_iter = def.hold_animations.begin(); anim_iter != def.hold_animations.end(); anim_iter++) {
            resolve_image_series(anim_iter->images);
        }
        for (auto anim_iter = def.move_animations.begin(); anim_iter != def.move_animations.end(); anim_iter++) {
            resolve_image_series(anim_iter->images);
        }
        for (auto anim_iter = def.attack_animations.begin(); anim_iter != def.attack_animations.end(); anim_iter++) {
            resolve_image_series(anim_iter->images);
        }
        for (auto anim_iter = def.recoil_animations.begin(); anim_iter != def.recoil_animations.end(); anim_iter++) {
            resolve_image_series(anim_iter->images);
        }
        for (auto anim_iter = def.die_animations.begin(); anim_iter != def.die_animations.end(); anim_iter++) {
            resolve_image_series(anim_iter->images);
        }
        for (auto anim_iter = def.shadow_animations.begin(); anim_iter != def.shadow_animations.end(); anim_iter++) {
            resolve_image_series(anim_iter->images);
        }

        for (auto sound_iter = def.sounds.begin(); sound_iter != def.sounds.end(); sound_iter++) {
            resolve_sound_series(sound_iter->sounds);
        }
    }

    for (auto def_iter = tile_view_defs.begin(); def_iter != tile_view_defs.end(); def_iter++) {
        TileViewDef& def = *def_iter->second;
        resolve_image_series(def.animation.images);
        for (auto trans_iter = def.transitions.begin(); trans_iter != def.transitions.end(); trans_iter++) {
            resolve_image_series(trans_iter->images);
        }
        resolve_image_series(def.roads);
        for (auto feat_iter = def.features.begin(); feat_iter != def.features.end(); feat_iter++) {
            resolve_image_series(feat_iter->images);
        }
    }

    for (auto def_iter = structure_view_defs.begin(); def_iter != structure_view_defs.end(); def_iter++) {
        StructureViewDef& def = *def_iter->second;
        resolve_image_series(def.animation.images);
    }
}

void Resources::resolve_image_series(std::vector<ImageRef>& image_series) {
    for (auto ref_iter = image_series.begin(); ref_iter != image_series.end(); ref_iter++) {
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

void Resources::resolve_sound_series(SoundSeries& sound_series) {
    for (auto ref_iter = sound_series.begin(); ref_iter != sound_series.end(); ref_iter++) {
        resolve_sound_ref(*ref_iter);
    }
}

bool Resources::resolve_sound_ref(SoundRef& sound_ref) {
    std::string sound_name = sound_ref.name;
    if (sounds.find(sound_name) == sounds.end()) {
        BOOST_LOG_TRIVIAL(warning) << "No sound for reference to: " << sound_name;
        return false;
    }
    sound_ref.sound = sounds[sound_name];
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
    TileViewDef::pointer base;
    if (def.base_name != def.name) {
        base = tile_view_defs.get(def.base_name);
    }
    return base;
}

std::string get_resource_basename(const std::string& filename) {
    size_t pos = filename.find_last_of("/");
    std::string basename(filename);
    if (pos != std::string::npos)
        basename = filename.substr(pos+1);
    std::transform(basename.begin(), basename.end(), basename.begin(), ::tolower);
    return basename;
}
