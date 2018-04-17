#ifndef RESOURCES_H
#define RESOURCES_H

#include "hexutil/basics/objmap.h"
#include "hexutil/messaging/loader.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"
#include "hexutil/scripting/scripting.h"

#include "hexav/audio/audio.h"
#include "hexav/graphics/graphics.h"
#include "hexav/resources/image_ref.h"
#include "hexav/resources/sound_ref.h"


namespace hex {

struct ImageLibraryResource {
    ImageLibraryResource(const std::string& path): path(path), loaded(false) { }

    std::string path;
    bool loaded;
    std::unordered_map<int, std::unique_ptr<Image> > images;
};


class ImageLoader;


class Resources {
public:
    Resources();
    virtual ~Resources();
    void clear();
    void resolve_image_series(std::vector<ImageRef>& image_series);
    bool resolve_image_ref(ImageRef& image_ref);
    void resolve_sound_series(std::vector<SoundRef>& sound_series);
    bool resolve_sound_ref(SoundRef& sound_ref);

    ImageLibraryResource *get_image_library(Atom name);
    Image *get_library_image(Atom library_name, int image_num);

public:
    ImageMap images;
    std::unordered_map<Atom, std::unique_ptr<ImageLibraryResource> > image_libraries;
    SoundMap sounds;
    std::unordered_map<Atom, ImageSeries> image_series;
    AtomMap<Script> scripts;
    std::set<std::string> songs;

protected:
    std::unique_ptr<ImageLoader> image_loader;

    friend class ResourceLoader;
};

};

#endif
