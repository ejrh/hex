#include "common.h"

#include "hex/resources/resources.h"


void SoundLoader::load(const std::string& filename) {
    std::string basename = get_resource_basename(filename);
    if (basename.rfind(".wav") == basename.size() - 4) {
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
