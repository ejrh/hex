#include "common.h"

#include "hexutil/basics/point.h"
#include "hexutil/basics/hexgrid.h"

#include "hexav/resources/paint.h"
#include "hexav/resources/resources.h"


static void add_bounds(SDL_Rect& bounds, const SDL_Rect& new_bounds) {
    if (bounds.x == INT_MAX) {
        bounds = new_bounds;
        return;
    }

    if (new_bounds.x + new_bounds.w > bounds.x + bounds.w) {
        bounds.w = new_bounds.x + new_bounds.w - bounds.x;
    }
    if (new_bounds.y + new_bounds.h > bounds.y + bounds.h) {
        bounds.h = new_bounds.y + new_bounds.h - bounds.y;
    }

    if (new_bounds.x < bounds.x) {
        bounds.w += bounds.x - new_bounds.x;
        bounds.x = new_bounds.x;
    }
    if (new_bounds.y < bounds.y) {
        bounds.h += bounds.y - new_bounds.y;
        bounds.y = new_bounds.y;
    }
}


int PaintItem::get_duration() const {
    return frames.size() * frame_rate_to_ms(frame_rate);
}

SDL_Rect PaintItem::get_bounds() const {
    SDL_Rect bounds = { INT_MAX, INT_MAX, 0, 0 };
    for (auto iter = frames.begin(); iter != frames.end(); iter++) {
        Image *image = *iter;
        SDL_Rect new_bounds = { image->clip_x_offset, image->clip_y_offset, image->clip_width, image->clip_height };
        add_bounds(bounds, new_bounds);
        break;
    }
    bounds.x += this->offset_x;
    bounds.y += this->offset_y;
    return bounds;
}


Paint::Paint():
        duration(0) {
    bounds.x = INT_MAX;
    bounds.y = INT_MAX;
    bounds.w = 0;
    bounds.h = 0;
}


void Paint::render(int x, int y, int phase, Graphics *graphics) {
    for (auto iter = items.begin(); iter != items.end(); iter++) {
        PaintItem& item = *iter;
        if (item.frames.size() == 0)
            continue;

        Image *frame = item.frames[0];
        if (item.frame_rate != -1) {
            int pos = (phase * item.frame_rate / FRAME_RATE_BASE);
            if (pos >= item.frames.size())
                pos %= item.frames.size();
            frame = item.frames[pos];
        }
        if (!frame)
            continue;

        bool restore_clip = false;
        SDL_Rect saved_clip_rect;

        int tile_count_x = (item.tile_width > 0) ? ((item.tile_width + frame->width) / frame->width) : 1;
        int tile_count_y = (item.tile_height > 0) ? ((item.tile_height + frame->height) / frame->height) : 1;
        if (item.tile_width > 0 || item.tile_height > 0 || item.clip_x > 0 || item.clip_y > 0) {
            int w = item.tile_width;
            int h = item.tile_height;
            if (w == 0)
                w = frame->width;
            if (h == 0)
                h = frame->height;
            SDL_Rect new_clip_rect = { x + item.offset_x + item.clip_x, y + item.offset_y + item.clip_y, w - item.clip_x, h - item.clip_y };
            saved_clip_rect = graphics->push_clip_rect(new_clip_rect);
            restore_clip = true;
        }

        for (int i = 0; i < tile_count_y; i++) {
            for (int j = 0; j < tile_count_x; j++) {
                int xp = x + item.offset_x + (j * frame->width);
                int yp = y + item.offset_y + (i * frame->height);

                if (item.blend_addition > 0) {
                    graphics->blit(frame, xp, yp, SDL_BLENDMODE_ADD, item.blend_addition);
                } else if (item.blend_addition < 0) {
                    graphics->blit(frame, xp, yp, SDL_BLENDMODE_MOD);
                } else {
                    if (item.blend_alpha != 0) {
                        graphics->blit(frame, xp, yp, SDL_BLENDMODE_BLEND, item.blend_alpha);
                    } else {
                        graphics->blit(frame, xp, yp, SDL_BLENDMODE_BLEND);
                    }
                }
            }
        }

        if (restore_clip)
            graphics->set_clip_rect(saved_clip_rect);
    }
    //const SDL_Rect& bounds = get_bounds();
    //graphics->draw_rectangle(255, 255, 255, x + bounds.x, y + bounds.y, bounds.w, bounds.h);
}

void Paint::clear() {
    items.clear();
    duration = 0;
    bounds.x = INT_MAX;
    bounds.y = INT_MAX;
    bounds.w = 0;
    bounds.h = 0;
}

void Paint::add(const PaintItem& item) {
    items.push_back(item);
    if (item.frame_rate != -1) {
        int item_duration = item.get_duration();
        if (item_duration > duration) {
            duration = item_duration;
        }
    }
    add_bounds(bounds, item.get_bounds());
}

int PaintExecution::frame_width(int frame_num) {
    Atom paint_library = get(paint_library_atom).get_as_atom();
    int frame_offset = get_as<int>(paint_frame_offset_atom);
    Image *image = resources->get_library_image(paint_library, frame_offset + frame_num);
    if (!image)
        return 0;
    return image->width;
}

int PaintExecution::frame_height(int frame_num) {
    Atom paint_library = get(paint_library_atom).get_as_atom();
    int frame_offset = get_as<int>(paint_frame_offset_atom);
    Image *image = resources->get_library_image(paint_library, frame_offset + frame_num);
    if (!image)
        return 0;
    return image->height;
}

void PaintExecution::paint_frame(int frame_num) {
    Atom paint_library = get(paint_library_atom).get_as_atom();
    int offset_x = get_as<int>(paint_offset_x_atom);
    int offset_y = get_as<int>(paint_offset_y_atom);
    int tile_width = get_as<int>(paint_tile_width_atom);
    int tile_height = get_as<int>(paint_tile_height_atom);
    int clip_x = get_as<int>(paint_clip_x_atom);
    int clip_y = get_as<int>(paint_clip_y_atom);
    int blend_alpha = get_as<int>(paint_blend_alpha_atom);
    int blend_addition = get_as<int>(paint_blend_addition_atom);
    int frame_offset = get_as<int>(paint_frame_offset_atom);
    paint_frame(paint_library, frame_offset + frame_num, offset_x, offset_y, tile_width, tile_height, clip_x, clip_y, blend_alpha, blend_addition);
}

void PaintExecution::paint_frame(Atom image_library, int frame_num, int offset_x, int offset_y, int tile_width, int tile_height, int clip_x, int clip_y, int blend_alpha, int blend_addition) {
    PaintItem pi;
    pi.offset_x = offset_x;
    pi.offset_y = offset_y;
    pi.tile_width = tile_width;
    pi.tile_height = tile_height;
    pi.clip_x = clip_x;
    pi.clip_y = clip_y;
    pi.frame_rate = -1;
    pi.blend_alpha = blend_alpha;
    pi.blend_addition = blend_addition;
    Image *image = resources->get_library_image(image_library, frame_num);
    pi.frames.push_back(image);
    paint->add(pi);
}

void PaintExecution::paint_animation(Atom image_library, int frame_time, const std::vector<int>& frame_nums, int offset_x, int offset_y, int blend_alpha, int blend_addition) {
    PaintItem pi;
    pi.offset_x = offset_x;
    pi.offset_y = offset_y;
    pi.tile_width = 0;
    pi.tile_height = 0;
    pi.clip_x = 0;
    pi.clip_y = 0;
    pi.frame_rate = frame_time;
    pi.blend_alpha = blend_alpha;
    pi.blend_addition = blend_addition;
    for (auto iter = frame_nums.begin(); iter != frame_nums.end(); iter++) {
        Image *image = resources->get_library_image(image_library, *iter);
        pi.frames.push_back(image);
    }
    paint->add(pi);
}

void PaintExecution::run(Script *script) {
    variables.set<int>(paint_offset_x_atom, 0);
    variables.set<int>(paint_offset_y_atom, 0);
    variables.set<int>(paint_tile_width_atom, 0);
    variables.set<int>(paint_tile_height_atom, 0);
    variables.set<int>(paint_clip_x_atom, 0);
    variables.set<int>(paint_clip_y_atom, 0);
    variables.set<int>(paint_blend_addition_atom, 0);
    variables.set<int>(paint_blend_alpha_atom, 0);
    variables.set<int>(paint_frame_offset_atom, 0);
    Execution::run(script);
}
