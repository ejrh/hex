#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include "hexutil/messaging/loader.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"

#include "hexview/resources/resources.h"


class ImageLoader {
public:
    ImageLoader(Resources *resources, Graphics *graphics): resources(resources), graphics(graphics) { }
    void load(const std::string& filename);
    void load_library(Atom name, const std::string& filename);

private:
    Resources *resources;
    Graphics *graphics;
};


class SoundLoader {
public:
    SoundLoader(Resources *resources, Audio *audio): resources(resources), audio(audio) { }
    void load(const std::string& filename);

private:
    Resources *resources;
    Audio *audio;
};


class ResourceLoader: public MessageLoader {
public:
    ResourceLoader(Resources *resources, ImageLoader *image_loader, SoundLoader *sound_loader):
            resources(resources), image_loader(image_loader), sound_loader(sound_loader),
            last_faction_view_def(), last_tile_view_def(), last_unit_view_def(), last_structure_view_def(),
            warned_image_loader(false), warned_sound_loader(false) { }

    void handle_message(Message *msg);

    void load_image(const std::string& filename);
    void load_image_library(Atom name, const std::string& filename);
    void load_song(const std::string& filename);
    void load_sound(const std::string& filename);
    void define_script(const std::string& name, MessageSequence& sequence);

private:
    Resources *resources;
    ImageLoader *image_loader;
    SoundLoader *sound_loader;
    FactionViewDef::pointer last_faction_view_def;
    TileViewDef::pointer last_tile_view_def;
    UnitViewDef::pointer last_unit_view_def;
    StructureViewDef::pointer last_structure_view_def;
    bool warned_image_loader;
    bool warned_sound_loader;

    Counter message_counter;
};


std::string get_resource_basename(const std::string& filename);
bool has_extension(const std::string& filename, const std::string& ext);

#endif
