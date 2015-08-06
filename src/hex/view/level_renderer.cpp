#include "common.h"

#include "hex/basics/error.h"
#include "hex/basics/vector2.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"
#include "hex/game/game.h"
#include "hex/view/view.h"
#include "hex/view/level_renderer.h"


LevelRenderer::LevelRenderer(Graphics *graphics, Resources *resources, Level *level, GameView *view):
        graphics(graphics), resources(resources), level(level), view(view) {
    cursor_images = resources->image_series["CURSORS"];
    arrow_images = resources->image_series["ARROWS"];
}

LevelRenderer::~LevelRenderer() {
}

void LevelRenderer::clear(int x, int y, int width, int height) {
    graphics->fill_rectangle(0,0,0, x, y, width, height);
}

void LevelRenderer::render_tile(int x, int y, Point tile_pos) {

    TileView& tile_view = view->level_view.tile_views[tile_pos];
    TileViewDef *def = tile_view.view_def;
    if (def == NULL)
        return;

    Image *ground = choose_image(def->animation.images, tile_view.phase / 1000);
    if (ground != NULL) {
        int alpha = (view->level_view.check_visibility(tile_pos)) ? 255 : 128;
        graphics->blit(ground, x, y, SDL_BLENDMODE_BLEND, alpha);
    }

    for (int i = 0; i < 3; i++) {
        Image *trans = tile_view.transition[i];
        if (trans != NULL) {
            int alpha = (view->level_view.check_visibility(tile_pos)) ? 255 : 128;
            graphics->blit(trans, x - 6, y - 12, SDL_BLENDMODE_BLEND, alpha);
        }
    }

    for (int i = 0; i < 6; i++) {
        Image *road = tile_view.road[i];
        if (road != NULL) {
            int alpha = (view->level_view.check_visibility(tile_pos)) ? 255 : 128;
            graphics->blit(road, x - 6, y - 12, SDL_BLENDMODE_BLEND, alpha);
        }
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
    if (!view->level_view.check_visibility(tile_pos))
        return;

    Tile &tile = level->tiles[tile_pos];
    UnitStack *stack = tile.stack;
    if (!stack)
        return;

    UnitStackView& stack_view = *view->get_unit_stack_view(*stack);
    if (stack_view.moving)
        return;

    draw_unit_stack(x, y, stack_view);
}

void LevelRenderer::draw_unit_stack(int x, int y, UnitStackView &stack_view) {
    UnitViewDef *view_def = stack_view.view_def;
    if (view_def == NULL) {
        return;
    }

    int facing = stack_view.facing;
    if (facing < 0 || facing >= 6)
        facing = 0;

    AnimationDef& animation = stack_view.moving ? view_def->move_animations[facing] : view_def->hold_animations[facing];
    if (animation.images.size() == 0)
        animation.images.push_back(ImageRef("missing"));
    Image *unit = animation.images[(stack_view.phase / 1000) % animation.images.size()].image;
    if (unit == NULL) {
        const std::string& label = view_def->name.substr(0, 3);
        TextFormat tf(graphics, SmallFont14, true, 255,255,255, 128,128,128);
        unit = tf.write_to_image(label);
        if (unit != NULL) {
            unit->x_offset = 24 - unit->width / 2 + 6;
            unit->y_offset = 16 - unit->height / 2 + 32;
            animation.images[(stack_view.phase / 1000) % animation.images.size()].image = unit;
        }
    }

    if (unit != NULL) {
        graphics->blit(unit, x - 8, y - 32, SDL_BLENDMODE_BLEND);
    }

    if (stack_view.selected && !stack_view.moving) {
        int add_phase = (view->phase / 1000) % 32;
        if (add_phase < 16)
            add_phase = add_phase*16;
        else {
            add_phase = (32 - add_phase)*16;
            if (add_phase > 255)
                add_phase = 255;
        }
        graphics->blit(unit, x - 8, y - 32, SDL_BLENDMODE_ADD, add_phase);
    }

    Faction *owner = stack_view.stack->owner;
    FactionView *faction_view = view->faction_views[owner->id];
    FactionViewDef *faction_view_def = faction_view->view_def;

    graphics->fill_rectangle(faction_view_def->r, faction_view_def->g, faction_view_def->b, x+32, y-12, 8, 12);
}

void LevelRenderer::draw_unit(int x, int y, Unit &unit, UnitViewDef *view_def) {
    int facing = 2;
    std::vector<ImageRef>& image_series = view_def->hold_animations[facing].images;
    if (image_series.size() == 0)
        image_series.push_back(ImageRef("missing"));
    Image *image = image_series[0].image;
    if (image == NULL) {
        const std::string& label = view_def->name.substr(0, 3);
        TextFormat tf(graphics, SmallFont14, true, 255,255,255, 128,128,128);
        image = tf.write_to_image(label);
        image->x_offset = 24 - image->width / 2 + 6;
        image->y_offset = 16 - image->height / 2 + 32;
        image_series[0].image = image;
    }

    graphics->blit(image, x - 8, y - 32, SDL_BLENDMODE_BLEND);
}

void LevelRenderer::render_path_arrow(int x, int y, Point tile_pos) {
    TileView &tile_view = view->level_view.tile_views[tile_pos];

    if (tile_view.path_dir >= 0 && tile_view.path_dir < 6) {
        Image *path_arrow = arrow_images[tile_view.path_dir].image;

        if (path_arrow != NULL)
            graphics->blit(path_arrow, x, y - 8, SDL_BLENDMODE_BLEND);
    }
}
