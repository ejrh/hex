#ifndef AUDIO_H
#define AUDIO_H

class Resources;
struct MODULE;

class Audio {
public:
    Audio(Resources *resources);
    ~Audio();

    void play(const std::string& filename);
    void update();

private:
    Resources *resources;
    MODULE *module;
};

#endif
