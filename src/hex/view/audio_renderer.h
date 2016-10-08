#ifndef AUDIO_RENDERER_H
#define AUDIO_RENDERER_H

class Audio;
class UnitView;

class AudioRenderer {
public:
    AudioRenderer(Audio *audio):
            audio(audio) { }

    void render_unit_sound(UnitView& unit_view);

private:
    Audio *audio;
};

#endif
