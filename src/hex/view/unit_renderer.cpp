#include "common.h"

#include "hex/basics/error.h"
#include "hex/basics/vector2.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"
#include "hex/view/unit_renderer.h"
#include "hex/view/view.h"


UnitRenderer::UnitRenderer(Graphics *graphics, Resources *resources):
        graphics(graphics), resources(resources), generate_placeholders(true) {
}

UnitRenderer::~UnitRenderer() {
}

void UnitRenderer::draw_unit(int x, int y, UnitView& unit_view) {
    // Shadow
    if (unit_view.posture != Dying) {
        AnimationDef& shadow_animation = unit_view.view_def->shadow_animations[unit_view.facing];
        if (shadow_animation.images.size() != 0) {
            int pos = unit_view.phase / 1000;
            Image *shadow = shadow_animation.images[pos % shadow_animation.images.size()].image;
            int alpha = 96;
            graphics->blit(shadow, x - shadow->width / 2, y - shadow->height + 8, SDL_BLENDMODE_BLEND, alpha);
        }
    }

    // Unit
    ImageSeries& image_series = (ImageSeries&) get_image_series(unit_view);
    Image *image = get_image_or_placeholder(image_series, unit_view.phase / 1000, unit_view.view_def->name);
    if (!image)
        return;

    x -= image->width / 2;
    y -= image->height;
    y += 8;

    graphics->blit(image, x, y, SDL_BLENDMODE_BLEND);

    // Highlight
    int highlight = 0;
    if (unit_view.selected) {
        int add_phase = (unit_view.phase / 1000) % 32;
        if (add_phase < 16)
            add_phase = add_phase*16;
        else {
            add_phase = (32 - add_phase)*16;
            if (add_phase > 255)
                add_phase = 255;
        }
        highlight = add_phase;
    }

    if (highlight != 0)
        graphics->blit(image, x, y, SDL_BLENDMODE_ADD, highlight);
}

void UnitRenderer::draw_unit_centered(int x, int y, UnitView& unit_view) {
    ImageSeries& image_series = (ImageSeries&) get_image_series(unit_view);
    Image *image = get_image_or_placeholder(image_series, unit_view.phase / 1000, unit_view.view_def->name);

    x -= image->clip_x_offset + image->clip_width / 2;
    y -= image->clip_y_offset + image->clip_height / 2;

    graphics->blit(image, x, y, SDL_BLENDMODE_BLEND);
    int highlight = 0;
    if (highlight != 0)
        graphics->blit(image, x, y, SDL_BLENDMODE_ADD, highlight);
}

const ImageSeries& UnitRenderer::get_image_series(const UnitView& unit_view) const {
    const AnimationDef& animation = unit_view.get_animation_def();
    return animation.images;
}

Image *UnitRenderer::get_image_or_placeholder(ImageSeries& image_series, int pos, const std::string name) {
    if (image_series.size() != 0) {
        return image_series[pos % image_series.size()].image;
    }
    if (!generate_placeholders)
        return NULL;

    const std::string& label = name.substr(0, 4);
    TextFormat tf(graphics, SmallFont14, false, 255,255,255, 128,128,128);
    Image *image = tf.write_to_image(label);
    image_series.push_back(ImageRef("PLACEHOLDER", image));
    return image;
}
