#include "common.h"

#include "hex/basics/error.h"
#include "hex/basics/vector2.h"
#include "hex/graphics/graphics.h"
#include "hex/game/game.h"
#include "hex/view/view.h"
#include "hex/view/level_renderer.h"

LevelRenderer::LevelRenderer(Graphics *graphics, Resources *resources, boost::shared_ptr<Level> level, LevelView *level_view):
        graphics(graphics), resources(resources), level(level), level_view(level_view) {
    cursor_images = resources->image_series["CURSORS"];
    arrow_images = resources->image_series["ARROWS"];
}

LevelRenderer::~LevelRenderer() {
}

void LevelRenderer::draw() {
    draw_level(&LevelRenderer::render_tile);
    draw_level(&LevelRenderer::render_unit_stack);
    draw_level(&LevelRenderer::render_path_arrow);
    draw_moving_unit();
}

void LevelRenderer::draw_level(RenderMethod render) {
    int x_spacing = level_view->x_spacing;
    int y_spacing = level_view->y_spacing;

    Point min_pos, max_pos;
    level_view->mouse_to_tile(0, 0, &min_pos);
    level_view->mouse_to_tile(level_view->width, level_view->height, &max_pos);

    if (min_pos.x % 2 == 1)
        min_pos.x--;
    if (min_pos.x < 0)
        min_pos.x = 0;
    min_pos.y--;
    if (min_pos.y < 0)
        min_pos.y = 0;
    max_pos.x++;
    if (max_pos.x > level->width)
        max_pos.x = level->width;
    max_pos.y++;
    if (max_pos.y > level->height)
        max_pos.y = level->height;

    for (int i = min_pos.y; i < max_pos.y; i++) {
        for (int j = min_pos.x; j < max_pos.x; j += 2) {

            int xpos = j*x_spacing - level_view->shift_x;
            int ypos = i*y_spacing - level_view->shift_y;
            (*this.*render)(xpos, ypos, Point(j, i));
        }

        for (int j = min_pos.x + 1; j < max_pos.x; j += 2) {
            int y_offset = y_spacing / 2;
            int xpos = j*x_spacing - level_view->shift_x;
            int ypos = i*y_spacing + y_offset - level_view->shift_y;
            (*this.*render)(xpos, ypos, Point(j, i));
        }
    }
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

    Tile &tile = level->tiles[tile_pos];
    UnitStack *stack = tile.stack;
    if (!stack || stack == level_view->moving_unit)
        return;

    UnitStackView& stack_view = *level_view->get_unit_stack_view(*stack);

    draw_unit_stack(x, y, stack_view);
}

void LevelRenderer::draw_moving_unit() {
    if (level_view->moving_unit == NULL)
        return;

    UnitStackView *stack_view = level_view->get_unit_stack_view(*level_view->moving_unit);

    int step = level_view->moving_unit_progress / 1000;
    Point prev_pos = level_view->moving_unit_path[step];
    Point next_pos = level_view->moving_unit_path[step + 1];
    int f = level_view->moving_unit_progress % 1000;

    int px1, py1, px2, py2;

    level_view->tile_to_pixel(prev_pos, &px1, &py1);
    level_view->tile_to_pixel(next_pos, &px2, &py2);

    int px = (px1 * (1000 - f) + px2 * f) / 1000;
    int py = (py1 * (1000 - f) + py2 * f) / 1000;
    bool selected = stack_view->selected;
    stack_view->selected = false;
    draw_unit_stack(px, py, *stack_view);
    stack_view->selected = selected;
}

void LevelRenderer::draw_unit_stack(int x, int y, UnitStackView &stack_view) {
    UnitViewDef *view_def = stack_view.view_def;
    if (view_def == NULL)
        return;

    int facing = stack_view.facing;
    if (facing < 0 || facing >= (int) view_def->images.size())
        return;

    std::vector<ImageRef> image_series = view_def->images[facing];
    Image *unit = image_series[(stack_view.phase / 1000) % image_series.size()].image;
    if (unit == NULL)
        return;

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
