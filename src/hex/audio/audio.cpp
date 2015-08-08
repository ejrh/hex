#include "common.h"

#include "hex/audio/audio.h"
#include "hex/basics/error.h"
#include "hex/resources/resources.h"

Audio::Audio(Resources *resources): resources(resources), module(NULL) {
    MikMod_RegisterAllDrivers();
    MikMod_RegisterAllLoaders();
    if (MikMod_Init((char *) "")) {
        throw Error("Couldn't initialise sound: %s", MikMod_strerror(MikMod_errno));
    }
}

Audio::~Audio() {
    Player_Stop();
    if (module != NULL)
        Player_Free(module);
    MikMod_Exit();
}

void Audio::play(const std::string& filename) {
    if (module != NULL) {
        Player_Free(module);
    }

    module = Player_Load((char *) filename.c_str(), 64, 0);
    if (!module) {
        warn("Couldn't load song: %s", MikMod_strerror(MikMod_errno));
        return;
    }

    trace("Playing: %s", filename.c_str());
    Player_Start(module);
}

void Audio::update() {
    if (!Player_Active() && resources->songs.size() > 0) {
        std::set<std::string>::iterator song_iter = resources->songs.begin();
        std::advance(song_iter, rand() % resources->songs.size());
        play(*song_iter);
    }

    MikMod_Update();
}
