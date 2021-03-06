#include "common.h"

#include "hexav/audio/audio.h"

#include "hexview/view/audio_renderer.h"
#include "hexview/view/view.h"


namespace hex {

void AudioRenderer::render_unit_sound(UnitView& unit_view) {
    if (!unit_view.play_sound)
        return;
    unit_view.play_sound = false;

    UnitViewDef& view_def = *unit_view.view_def;

    Sound *sound = choose_sound(view_def.sounds[unit_view.posture].sounds, unit_view.phase);
    if (sound != NULL) {
        audio->play_sound(*sound);
    }
}

};
