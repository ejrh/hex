#ifndef RESOURCES_H
#define RESOURCES_H

#include "hexutil/basics/objmap.h"
#include "hexutil/messaging/loader.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"

#include "hexav/graphics/graphics.h"

#include "hexview/resources/view_def.h"


class Resources {
public:
    Resources():
            tile_view_defs("tile_view_defs"), unit_view_defs("unit_view_defs"), structure_view_defs("structure_view_defs"),
            faction_view_defs("faction_view_defs") { }
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

private:
    TileViewDef::pointer find_base(const TileViewDef& def) const;

public:
    ImageMap images;
    SoundMap sounds;
    std::map<std::string, ImageSeries> image_series;
    StrMap<TileViewDef> tile_view_defs;
    StrMap<UnitViewDef> unit_view_defs;
    StrMap<StructureViewDef> structure_view_defs;
    StrMap<FactionViewDef> faction_view_defs;
    std::set<std::string> songs;

    friend class ResourceLoader;
};


#endif
