#ifndef AUDIO_H
#define AUDIO_H

class Resources;

// Can't define these if also including SDL_mixer.h
#ifndef _SDL_MIXER_H
struct Mix_Music;
struct Mix_Chunk;
#endif

class Sound {
public:
    Sound();
    ~Sound();
public:
    Mix_Chunk *chunk;
};

typedef std::map<std::string, Sound *> SoundMap;


class Audio {
public:
    Audio(Resources *resources);
    ~Audio();

    void start();
    void stop();
    void play(const std::string& filename);
    void play_sound(Sound& sound);
    void update();
    Sound *load_sound(const std::string& filename);

private:
    Resources *resources;
    boost::atomic<bool> started;
    Mix_Music *song;
    bool no_music, no_sound;
};

#endif
