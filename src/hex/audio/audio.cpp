#include "common.h"

#include <SDL2/SDL_mixer.h>

#define AUDIO_CPP
#include "hex/audio/audio.h"
#include "hex/basics/error.h"
#include "hex/resources/resources.h"

Sound::Sound():
        chunk(NULL) {
}

Sound::~Sound() {
    if (chunk) {
        Mix_FreeChunk(chunk);
    }
}

Audio::Audio(Resources *resources): resources(resources), started(false), song(NULL), no_music(false), no_sound(false) {
}

Audio::~Audio() {
    stop();
}

void Audio::start() {
    if (started)
        return;

    if (!(Mix_Init(MIX_INIT_MODPLUG) & MIX_INIT_MODPLUG)) {
        BOOST_LOG_TRIVIAL(error) << "Couldn't initialise music player: " << Mix_GetError();
        no_music = true;
    }
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        BOOST_LOG_TRIVIAL(error) << "Couldn't initialise audio: " << Mix_GetError();
        no_sound = true;
        no_music = true;
    }

    started = true;
}

void Audio::stop() {
    if (!started)
        return;

    if (song != NULL) {
        Mix_FreeMusic(song);
        song = NULL;
    }

    Mix_CloseAudio();
}

void Audio::play(const std::string& filename) {
    if (!started || no_music)
        return;

    if (song != NULL) {
        Mix_FreeMusic(song);
    }

    song = Mix_LoadMUS(filename.c_str());
    if (!song) {
        BOOST_LOG_TRIVIAL(warning) << "Couldn't load song: " << Mix_GetError();
        return;
    }

    if (Mix_PlayMusic(song, 0) < 0) {
        BOOST_LOG_TRIVIAL(warning) << "Couldn't play music: " << Mix_GetError();
    }
}

void Audio::play_sound(Sound& sound) {
    if (!started || no_sound)
        return;

    Mix_PlayChannel(-1, sound.chunk, 0);
}

void Audio::update() {
    if (!started)
        return;

    if (!Mix_PlayingMusic() && resources->songs.size() > 0) {
        auto song_iter = resources->songs.begin();
        std::advance(song_iter, rand() % resources->songs.size());
        play(*song_iter);
    }
}

Sound *Audio::load_sound(const std::string& filename) {
    Mix_Chunk *chunk = Mix_LoadWAV(filename.c_str());
    if (!chunk) {
        BOOST_LOG_TRIVIAL(warning) << "Couldn't load sound: " << Mix_GetError();
        return NULL;
    }

    Sound *sound = new Sound();
    sound->chunk = chunk;
    return sound;
}
