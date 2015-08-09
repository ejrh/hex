#ifndef AUDIO_H
#define AUDIO_H

class Resources;
struct MODULE;

class Audio {
public:
    Audio(Resources *resources);
    ~Audio();

    void start();
    void stop();
    void play(const std::string& filename);
    void update();

private:
    Resources *resources;
    bool started;
    MODULE *module;
};

#endif
