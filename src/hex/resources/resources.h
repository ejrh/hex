#ifndef RESOURCES_H
#define RESOURCES_H

#include "hex/basics/objmap.h"
#include "hex/graphics/graphics.h"
#include "hex/messaging/loader.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"
#include "hex/resources/view_def.h"


class Resources {
public:
    Resources():
            tile_view_defs("tile_view_defs"), unit_view_defs("unit_view_defs"), structure_view_defs("structure_view_defs"),
            faction_view_defs("faction_view_defs") { }
    virtual ~Resources() { }
    void resolve_references();
    void resolve_image_series(std::vector<ImageRef>& image_series);
    bool resolve_image_ref(ImageRef& image_ref);

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
    std::map<std::string, ImageSeries> image_series;
    StrMap<TileViewDef> tile_view_defs;
    StrMap<UnitViewDef> unit_view_defs;
    StrMap<StructureViewDef> structure_view_defs;
    StrMap<FactionViewDef> faction_view_defs;
    std::set<std::string> songs;

    friend class ResourceLoader;
};


class ImageLoader {
public:
    ImageLoader(Resources *resources, Graphics *graphics): resources(resources), graphics(graphics) { }
    void load(const std::string& filename);

private:
    Resources *resources;
    Graphics *graphics;
};


class ResourceLoader: public MessageLoader {
public:
    ResourceLoader(Resources *resources, ImageLoader *image_loader): resources(resources), image_loader(image_loader),
            last_faction_view_def(), last_tile_view_def(), last_unit_view_def(), last_structure_view_def(), warned_image_loader(false) { }

    void handle_message(boost::shared_ptr<Message> msg);

    void load_image(const std::string& filename);
    void load_song(const std::string& filename);

private:
    Resources *resources;
    ImageLoader *image_loader;
    FactionViewDef::pointer last_faction_view_def;
    TileViewDef::pointer last_tile_view_def;
    UnitViewDef::pointer last_unit_view_def;
    StructureViewDef::pointer last_structure_view_def;
    bool warned_image_loader;
};


#endif
