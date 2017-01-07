#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/basics/vector2.h"

#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"

#include "hexview/view/unit_renderer.h"
#include "hexview/view/view.h"


UnitRenderer::UnitRenderer(Graphics *graphics, Resources *resources):
        generate_placeholders(true),
        graphics(graphics), resources(resources) {
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
            graphics->blit(shadow, x - shadow->width / 2, y - shadow->height + 24, SDL_BLENDMODE_BLEND, alpha);
        }
    }

    // Unit
    ImageSeries& image_series = (ImageSeries&) get_image_series(unit_view);
    Image *image = get_image_or_placeholder(image_series, unit_view.phase / 1000, unit_view.view_def->name);
    if (!image)
        return;

    x -= image->width / 2;
    y -= image->height;
    y += 24;

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

void UnitRenderer::draw_health_bar(int x, int y, int w, int h, int health, int max_health) {
    static const int R[] = { 255, 228, 0 };
    static const int G[] = { 0, 228, 255 };
    static const int B[] = { 0, 0, 0 };

    graphics->fill_rectangle(0, 0, 0, x, y, w, h);
    if (health == 0)
        return;
    // width
    float f = health / (float) max_health;
    int w2 = f * (w - 2);
    // colour
    float f2 = (health - 1) / (float) (max_health - 1);
    float f2b = (f2 < 0.5) ? (f2 * 2) : ((f2 - 0.5) * 2);
    float f2a = 1.0 - f2b;
    int r = (f2 < 0.5) ? (R[0]*f2a + R[1]*f2b) : (R[1]*f2a + R[2]*f2b);
    int g = (f2 < 0.5) ? (G[0]*f2a + G[1]*f2b) : (G[1]*f2a + G[2]*f2b);
    int b = (f2 < 0.5) ? (B[0]*f2a + B[1]*f2b) : (B[1]*f2a + B[2]*f2b);
    graphics->fill_rectangle(r, g, b, x + 1, y + 1, w2, h - 2);
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
    TextFormat tf(SmallFont14, false, 255,255,255, 128,128,128);
    Image *image = tf.write_to_image(graphics, label);
    image_series.push_back(ImageRef("PLACEHOLDER", image));
    return image;
}
