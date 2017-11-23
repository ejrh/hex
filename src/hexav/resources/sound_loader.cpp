#include "common.h"

#include "hexav/resources/resources.h"
#include "hexav/resources/resource_loader.h"


void SoundLoader::load(const std::string& filename) {
    std::string basename = get_resource_basename(filename);
    if (has_extension(basename, ".wav")) {
        BOOST_LOG_TRIVIAL(info) << "Loading sound from: " << filename;
        Sound *sound = audio->load_sound(filename);
        if (!sound) {
            BOOST_LOG_TRIVIAL(error) << "Could not load sound: " << filename;
            return;
        }
        resources->sounds[basename] = sound;
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Don't know how to load sound from: " << filename;
    }
}
