#ifndef SOUND_REF_H
#define SOUND_REF_H

#include "hexutil/messaging/serialiser.h"


namespace hex {

class Sound;

struct SoundRef {
    SoundRef(): name(), sound(NULL) { }
    SoundRef(std::string name): name(name), sound(NULL) { }
    SoundRef(std::string name, Sound *sound): name(name), sound(sound) { }
    std::string name;
    Sound *sound;
};

typedef std::vector<SoundRef> SoundSeries;

inline Sound *choose_sound(SoundSeries& series, int num) {
    if (series.size() == 0)
        return NULL;
    SoundRef& sound_ref = series[num % series.size()];
    return sound_ref.sound;
}

inline Serialiser& operator<<(Serialiser& serialiser, const SoundRef& r) {
    serialiser << r.name;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, SoundRef& r) {
    deserialiser >> r.name;
    r.sound = NULL;
    return deserialiser;
}

};

#endif
