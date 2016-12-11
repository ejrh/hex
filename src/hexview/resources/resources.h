#ifndef RESOURCES_H
#define RESOURCES_H

#include "hexutil/basics/objmap.h"
#include "hexutil/messaging/loader.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"
#include "hexutil/scripting/scripting.h"

#include "hexav/graphics/graphics.h"

#include "hexview/resources/view_def.h"


struct ImageLibraryResource {
    ImageLibraryResource(const std::string& path): path(path) { }

    std::string path;
    bool loaded;
    std::unordered_map<int, std::unique_ptr<Image> > images;
};


class Resources {
public:
    Resources():
            tile_view_defs("tile_view_defs"), unit_view_defs("unit_view_defs"), structure_view_defs("structure_view_defs"),
            faction_view_defs("faction_view_defs"), scripts("scripts") { }
    virtual ~Resources() { }
    void resolve_references();
    void resolve_image_series(std::vector<ImageRef>& image_series);
    bool resolve_image_ref(ImageRef& image_ref);
    void resolve_sound_series(std::vector<SoundRef>& sound_series);
    bool resolve_sound_ref(SoundRef& sound_ref);

    TileViewDef::pointer create_tile_view(const TileViewDef& def);
    StructureViewDef::pointer create_structure_view(const StructureViewDef& data);
    TileViewDef::pointer get_tile_view_def(const std::string& name);
    UnitViewDef::pointer get_unit_view_def(const std::string& name);
    StructureViewDef::pointer get_structure_view_def(const std::string& name);
    FactionViewDef::pointer get_faction_view_def(const std::string& name);

    ImageLibraryResource *get_image_library(Atom name);
    Image *get_library_image(Atom library_name, int image_num);

private:
    TileViewDef::pointer find_base(const TileViewDef& def) const;

public:
    ImageMap images;
    std::map<Atom, std::unique_ptr<ImageLibraryResource> > image_libraries;
    SoundMap sounds;
    std::map<std::string, ImageSeries> image_series;
    StrMap<TileViewDef> tile_view_defs;
    StrMap<UnitViewDef> unit_view_defs;
    StrMap<StructureViewDef> structure_view_defs;
    StrMap<FactionViewDef> faction_view_defs;
    StrMap<Script> scripts;
    std::set<std::string> songs;

    friend class ResourceLoader;
};


#endif