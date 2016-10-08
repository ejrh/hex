#ifndef AUDIO_H
#define AUDIO_H

class Resources;
struct MODULE;
struct SAMPLE;

class Sound {
public:
    Sound();
    ~Sound();
public:
    SAMPLE *sample;
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
    void run_thread();

private:
    Resources *resources;
    boost::atomic<bool> started;
    MODULE *module;
    boost::thread audio_thread;
};

#endif
