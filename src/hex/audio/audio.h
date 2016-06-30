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
    void run_thread();

private:
    Resources *resources;
    boost::atomic<bool> started;
    MODULE *module;
    boost::thread audio_thread;
};

#endif
