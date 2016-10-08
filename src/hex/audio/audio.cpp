#include "common.h"

#include "hex/audio/audio.h"
#include "hex/basics/error.h"
#include "hex/resources/resources.h"

Sound::Sound():
        sample(NULL) {
}

Sound::~Sound() {
    if (sample) {
        Sample_Free(sample);
    }
}

Audio::Audio(Resources *resources): resources(resources), started(false), module(NULL) {
}

Audio::~Audio() {
    stop();
}

void Audio::start() {
    if (started)
        return;

    MikMod_RegisterAllDrivers();

#ifdef _WIN32
    // Tell DirectSound to use globalfocus so it plays in the background
    int ds_id = MikMod_DriverFromAlias("ds");
    if (ds_id != 0) {
        MDRIVER *ds = MikMod_DriverByOrdinal(ds_id);
        ds->CommandLine("globalfocus=1");
    }
#endif

    MikMod_RegisterAllLoaders();
    if (MikMod_Init((char *) "")) {
        throw Error() << "Couldn't initialise sound: " << MikMod_strerror(MikMod_errno);
    }

    BOOST_LOG_TRIVIAL(debug) << "MikMod drivers: " << MikMod_InfoDriver();

    MikMod_SetNumVoices(-1, 2);

    started = true;
    audio_thread = boost::thread(&Audio::run_thread, this);
}

void Audio::stop() {
    if (!started)
        return;

    started = false;
    audio_thread.join();

    Player_Stop();
    if (module != NULL)
        Player_Free(module);
    MikMod_Exit();
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

void Audio::play_sound(Sound& sound) {
    Sample_Play(sound.sample, 0, 0);
}

void Audio::update() {
    if (!started)
        return;

    if (!Player_Active() && resources->songs.size() > 0) {
        std::set<std::string>::iterator song_iter = resources->songs.begin();
        std::advance(song_iter, rand() % resources->songs.size());
        play(*song_iter);
    }
}

Sound *Audio::load_sound(const std::string& filename) {
    SAMPLE *sample = Sample_Load((char *) filename.c_str());
    if (!sample) {
        BOOST_LOG_TRIVIAL(warning) << "Couldn't load sound: " << MikMod_strerror(MikMod_errno);
        return NULL;
    }
    Sound *sound = new Sound();
    sound->sample = sample;
    return sound;
}

void Audio::run_thread() {
    while (started) {
        MikMod_Update();
        SDL_Delay(20);
    }
}
