#include "hexview/resources/paint.h"
#include "hexview/resources/resources.h"

void Paint::render(int x, int y, int phase, Graphics *graphics) {
    for (auto iter = items.begin(); iter != items.end(); iter++) {
        PaintItem& item = *iter;
        if (item.frames.size() == 0)
            continue;

        Image *frame = item.frames[0];
        if (item.frames.size() > 1) {
            int pos = (phase * item.frame_rate / 4000) % item.frames.size();
            frame = item.frames[pos];
        }
        if (!frame)
            continue;

        if (item.blend_addition != 0) {
            graphics->blit(frame, x + item.offset_x, y + item.offset_y, SDL_BLENDMODE_ADD, item.blend_addition);
        } else {
            graphics->blit(frame, x + item.offset_x, y + item.offset_y, SDL_BLENDMODE_BLEND);
        }
    }
}

void PaintExecution::paint_frame(Atom image_library, int frame_num, int offset_x, int offset_y, int blend_addition) {
    PaintItem pi;
    pi.offset_x = offset_x;
    pi.offset_y = offset_y;
    pi.frame_rate = 0;
    pi.blend_addition = blend_addition;
    Image *image = resources->get_library_image(image_library, frame_num);
    pi.frames.push_back(image);
    paint->items.push_back(pi);
}

void PaintExecution::paint_animation(Atom image_library, int frame_rate, std::vector<int>& frame_nums, int offset_x, int offset_y, int blend_addition) {
    PaintItem pi;
    pi.offset_x = offset_x;
    pi.offset_y = offset_y;
    pi.frame_rate = frame_rate;
    pi.blend_addition = blend_addition;
    for (auto iter = frame_nums.begin(); iter != frame_nums.end(); iter++) {
        Image *image = resources->get_library_image(image_library, *iter);
        pi.frames.push_back(image);
    }
    paint->items.push_back(pi);
}
