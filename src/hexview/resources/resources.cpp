#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/basics/logging.h"

#include "hexview/resources/resources.h"
#include "hexview/resources/resource_loader.h"


Resources::Resources():
        tile_view_defs("tile_view_defs"), feature_view_defs("feature_view_defs"),
        unit_view_defs("unit_view_defs"), structure_view_defs("structure_view_defs"),
        faction_view_defs("faction_view_defs"), scripts("scripts") {
}


Resources::~Resources() { }


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
        for (auto sound_iter = def.sounds.begin(); sound_iter != def.sounds.end(); sound_iter++) {
            resolve_sound_series(sound_iter->sounds);
        }
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
    return def;
}

FeatureViewDef::pointer Resources::create_feature_view(const FeatureViewDef& data) {
    FeatureViewDef::pointer def = boost::make_shared<FeatureViewDef>(data);
    feature_view_defs.put_and_warn(def->name, def);
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

FeatureViewDef::pointer Resources::get_feature_view_def(const std::string& name) {
    return feature_view_defs.get_and_warn(name);
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

ImageLibraryResource *Resources::get_image_library(Atom name) {
    auto found = image_libraries.find(name);
    if (found == image_libraries.end()) {
        return nullptr;
    }

    ImageLibraryResource *image_lib = found->second.get();
    if (!image_lib->loaded) {
        image_loader->load_library(name, image_lib->path);
    }
    return image_lib;
}

Image *Resources::get_library_image(Atom library_name, int image_num) {
    static std::unordered_set<Atom> library_warnings;
    static std::unordered_set<std::pair<Atom, int>> library_image_warnings;

    ImageLibraryResource *lib = get_image_library(library_name);
    if (!lib) {
        log_once(warning, library_warnings, library_name, "No image library: " << library_name);
        return nullptr;
    }
    auto found = lib->images.find(image_num);
    if (found == lib->images.end()) {
        log_once(warning, library_image_warnings, std::make_pair(library_name, image_num), "No image at position " << image_num << " in library " << library_name);
        return nullptr;
    }
    return found->second.get();
}
