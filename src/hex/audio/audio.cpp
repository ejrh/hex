#include "common.h"

#include "hex/audio/audio.h"
#include "hex/basics/error.h"
#include "hex/resources/resources.h"

Audio::Audio(Resources *resources): resources(resources), started(false), module(NULL) {
}

Audio::~Audio() {
    stop();
}

void Audio::start() {
    if (started)
        return;

    MikMod_RegisterAllDrivers();
    MikMod_RegisterAllLoaders();
    if (MikMod_Init((char *) "")) {
        throw Error("Couldn't initialise sound: %s", MikMod_strerror(MikMod_errno));
    }
    started = true;
}

void Audio::stop() {
    if (!started)
        return;

    Player_Stop();
    if (module != NULL)
        Player_Free(module);
    MikMod_Exit();
    started = false;
}

void Audio::play(const std::string& filename) {
    if (!started)
        return;

    if (module != NULL) {
        Player_Free(module);
    }

    module = Player_Load((char *) filename.c_str(), 64, 0);
    if (!module) {
        BOOST_LOG_TRIVIAL(warning) << "Couldn't load song: " << MikMod_strerror(MikMod_errno);
        return;
    }

    module->loop = 0;
    BOOST_LOG_TRIVIAL(debug) << "Playing: " << filename;
    Player_Start(module);
}

void Audio::update() {
    if (!started)
        return;

    if (!Player_Active() && resources->songs.size() > 0) {
        std::set<std::string>::iterator song_iter = resources->songs.begin();
        std::advance(song_iter, rand() % resources->songs.size());
        play(*song_iter);
    }

    MikMod_Update();
}
