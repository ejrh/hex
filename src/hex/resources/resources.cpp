#include "common.h"

#include "hex/basics/error.h"
#include "hex/resources/resources.h"


void Resources::resolve_references() {
    for (std::map<std::string, ImageSeries>::iterator iter = image_series.begin(); iter != image_series.end(); iter++) {
        resolve_image_series(iter->second);
    }

    for (UnitViewDefMap::iterator def_iter = unit_view_defs.begin(); def_iter != unit_view_defs.end(); def_iter++) {
        UnitViewDef *def = def_iter->second;
        for (UnitImageMap::iterator series_iter = def->images.begin(); series_iter != def->images.end(); series_iter++) {
            resolve_image_series(*series_iter);
        }
    }

    for (TileViewDefMap::iterator def_iter = tile_view_defs.begin(); def_iter != tile_view_defs.end(); def_iter++) {
        TileViewDef *def = def_iter->second;
        for (TileImageMap::iterator series_iter = def->images.begin(); series_iter != def->images.end(); series_iter++) {
            resolve_image_series(*series_iter);
        }
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

FactionViewDef *Resources::get_faction_view_def(const std::string& name) {
    FactionViewDefMap::const_iterator iter = faction_view_defs.find(name);
    if (iter != faction_view_defs.end())
        return iter->second;

    return NULL;
}
