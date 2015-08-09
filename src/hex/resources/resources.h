#ifndef RESOURCES_H
#define RESOURCES_H

#include "hex/graphics/graphics.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"
#include "hex/messaging/updater.h"
#include "hex/resources/view_def.h"


class Resources {
public:
    Resources() { }
    virtual ~Resources() { }
    void resolve_references();
    void resolve_image_series(std::vector<ImageRef>& image_series);
    bool resolve_image_ref(ImageRef& image_ref);

    TileViewDef *create_tile_view(const TileViewDef& def);
    StructureViewDef *create_structure_view(const StructureViewDef& data);
    TileViewDef *get_tile_view_def(const std::string& name) const;
    UnitViewDef *get_unit_view_def(const std::string& name);
    StructureViewDef *get_structure_view_def(const std::string& name) const;
    FactionViewDef *get_faction_view_def(const std::string& name);

private:
    TileViewDef *find_base(const TileViewDef& def) const;

public:
    ImageMap images;
    std::map<std::string, ImageSeries> image_series;
    TileViewDefMap tile_view_defs;
    UnitViewDefMap unit_view_defs;
    StructureViewDefMap structure_view_defs;
    FactionViewDefMap faction_view_defs;
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


class ResourceLoader: public MessageReceiver {
public:
    ResourceLoader(Resources *resources, ImageLoader *image_loader): resources(resources), image_loader(image_loader),
            last_tile_view_def(NULL), last_unit_view_def(NULL), last_structure_view_def(NULL) { }

    void receive(boost::shared_ptr<Message> msg);

    void load(const std::string& filename);
    void include(const std::string& filename, bool skip_missing = false);
    void load_image(const std::string& filename);
    void load_song(const std::string& filename);

private:
    Resources *resources;
    ImageLoader *image_loader;
    TileViewDef *last_tile_view_def;
    UnitViewDef *last_unit_view_def;
    StructureViewDef *last_structure_view_def;
    std::vector<std::string> current_files;
};


#endif
