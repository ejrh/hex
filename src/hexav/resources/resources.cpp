#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/basics/logging.h"

#include "hexav/resources/resources.h"
#include "hexav/resources/resource_loader.h"


Resources::Resources():
        scripts("scripts") {
}


Resources::~Resources() { }


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
