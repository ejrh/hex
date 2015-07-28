#include "common.h"

#include "hex/basics/error.h"
#include "hex/basics/vector2.h"
#include "hex/graphics/graphics.h"
#include "hex/graphics/font.h"
#include "hex/game/game.h"
#include "hex/view/view.h"
#include "hex/view/level_renderer.h"

#define TILE_WIDTH 48
#define X_SPACING 32
#define Y_SPACING 32

#define SLOPE_WIDTH (TILE_WIDTH - X_SPACING)
#define SLOPE_HEIGHT (Y_SPACING/2)


LevelRenderer::LevelRenderer(Graphics *graphics, Resources *resources, Level *level, LevelView *level_view):
        graphics(graphics), resources(resources), level(level), level_view(level_view), x_spacing(X_SPACING), y_spacing(Y_SPACING) {
    cursor_images = resources->image_series["CURSORS"];
    arrow_images = resources->image_series["ARROWS"];
}

LevelRenderer::~LevelRenderer() {
}

void LevelRenderer::clear(int x, int y, int width, int height) {
    graphics->fill_rectangle(0,0,0, x, y, width, height);
}

void LevelRenderer::render_tile(int x, int y, Point tile_pos) {

    TileView& tile_view = level_view->tile_views[tile_pos];
    TileViewDef *def = tile_view.view_def;
    if (def == NULL)
        return;

    std::vector<ImageRef>& image_series = def->images[tile_view.variation % def->images.size()];
    ImageRef& image_ref = image_series[tile_view.phase % image_series.size()];
    Image *ground = image_ref.image;

    if (ground != NULL)
        graphics->blit(ground, x, y);

    if (!level->visibility.check(tile_pos)) {
        graphics->fill_rectangle(0,0,0, x+24 - 8, y+16 - 8, 16, 16);
    }

    if (tile_view.highlighted) {
        Image *highlight1 = cursor_images[0].image;
        if (highlight1 != NULL)
            graphics->blit(highlight1, x, y);
            //graphics->blit(highlight1, x, y - 32);
    }

    /*if (tile_view.highlighted) {
        Image *highlight2 = cursor_images[1].image;
        if (highlight2 != NULL)
            graphics->blit(highlight2, x, y - 32);
    }*/
}

void LevelRenderer::render_unit_stack(int x, int y, Point tile_pos) {
    if (!level->visibility.check(tile_pos))
        return;

    Tile &tile = level->tiles[tile_pos];
    UnitStack *stack = tile.stack;
    if (!stack || stack == level_view->moving_unit)
        return;

    UnitStackView& stack_view = *level_view->get_unit_stack_view(*stack);

    draw_unit_stack(x, y, stack_view);
}

void LevelRenderer::draw_unit_stack(int x, int y, UnitStackView &stack_view) {
    UnitViewDef *view_def = stack_view.view_def;
    if (view_def == NULL)
        return;

    int facing = stack_view.facing;
    if (facing < 0 || facing >= (int) view_def->images.size())
        return;

    std::vector<ImageRef>& image_series = view_def->images[facing];
    Image *unit = image_series[(stack_view.phase / 1000) % image_series.size()].image;
    if (unit == NULL) {
        const std::string& label = view_def->name.substr(0, 3);
        TextFormat tf(graphics, SmallFont14, true, 255,255,255, 128,128,128);
        unit = tf.write_to_image(label);
        unit->x_offset = 24 - unit->width / 2 + 6;
        unit->y_offset = 16 - unit->height / 2 + 32;
        image_series[(stack_view.phase / 1000) % image_series.size()].image = unit;
    }

    int alpha;
    if (stack_view.selected) {
        int alpha_phase = (stack_view.phase / 2000) % 32;
        if (alpha_phase < 16)
            alpha = alpha_phase*16;
        else
            alpha = (31 - alpha_phase)*16;
    } else {
        alpha = 255;
    }

    graphics->blit(unit, x - 8, y - 32, alpha);
}

void LevelRenderer::render_path_arrow(int x, int y, Point tile_pos) {
    TileView &tile_view = level_view->tile_views[tile_pos];

    if (tile_view.path_dir >= 0 && tile_view.path_dir < 6) {
        Image *path_arrow = arrow_images[tile_view.path_dir].image;

        if (path_arrow != NULL)
            graphics->blit(path_arrow, x, y - 8);
    }
}
