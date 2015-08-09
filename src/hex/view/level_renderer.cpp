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
        graphics->blit(ground, x - ground->width / 2, y - ground->height / 2, SDL_BLENDMODE_BLEND, alpha);
    }
}

void LevelRenderer::render_tile_transitions(int x, int y, Point tile_pos) {
    TileView& tile_view = view->level_view.tile_views[tile_pos];

    for (std::vector<Image *>::iterator iter = tile_view.transitions.begin(); iter != tile_view.transitions.end(); iter++) {
        Image *trans = *iter;
        if (trans != NULL) {
            int alpha = (view->level_view.check_visibility(tile_pos)) ? 255 : 128;
            int trans_x = x - trans->width / 2;
            int trans_y = y - trans->height / 2 - 6;
            graphics->blit(trans, trans_x, trans_y, SDL_BLENDMODE_BLEND, alpha);
        }
    }

    if (tile_view.feature != NULL) {
        int alpha = (view->level_view.check_visibility(tile_pos)) ? 255 : 128;
        int feature_x = x - tile_view.feature_x;
        int feature_y = y - tile_view.feature_y;
        graphics->blit(tile_view.feature, feature_x, feature_y, SDL_BLENDMODE_BLEND, alpha);
    }
}

void LevelRenderer::render_structure(int x, int y, Point tile_pos) {
    TileView& tile_view = view->level_view.tile_views[tile_pos];

    for (std::vector<Image *>::iterator iter = tile_view.roads.begin(); iter != tile_view.roads.end(); iter++) {
        Image *road = *iter;
        if (road != NULL) {
            int alpha = (view->level_view.check_visibility(tile_pos)) ? 255 : 128;
            graphics->blit(road, x - road->width / 2, y - road->height / 2, SDL_BLENDMODE_BLEND, alpha);
        }
    }

    if (tile_view.structure_view != NULL) {
        StructureViewDef *view_def = tile_view.structure_view->view_def;
        AnimationDef& animation = view_def->animation;
        if (animation.images.size() == 0)
            animation.images.push_back(ImageRef("missing"));
        Image *structure = animation.images[(tile_view.phase / 1000) % animation.images.size()].image;
        if (structure == NULL) {
            const std::string& label = view_def->name.substr(0, 3);
            TextFormat tf(graphics, SmallFont14, false, 255,255,255, 128,128,128);
            structure = tf.write_to_image(label);
            if (structure != NULL) {
                animation.images[(tile_view.phase / 1000) % animation.images.size()].image = structure;
            }
        }
        int alpha = (view->level_view.check_visibility(tile_pos)) ? 255 : 128;
        graphics->blit(structure, x - view_def->centre_x, y - view_def->centre_y, SDL_BLENDMODE_BLEND, alpha);

        if (tile_view.structure_view->selected) {
            int add_phase = (view->phase / 1000) % 32;
            if (add_phase < 16)
                add_phase = add_phase*16;
            else {
                add_phase = (32 - add_phase)*16;
                if (add_phase > 255)
                    add_phase = 255;
            }
            graphics->blit(structure, x - view_def->centre_x, y - view_def->centre_y, SDL_BLENDMODE_ADD, add_phase);
        }

        Faction *owner = tile_view.structure_view->structure->owner;
        FactionView *faction_view = view->faction_views[owner->id];
        FactionViewDef *faction_view_def = faction_view->view_def;

        graphics->fill_rectangle(faction_view_def->r, faction_view_def->g, faction_view_def->b, x+16, y-20, 8, 12);
    }
}

void LevelRenderer::render_unit_stack(int x, int y, Point tile_pos) {
    bool draw_it = true;
    if (!view->level_view.check_visibility(tile_pos))
        return;

    Tile &tile = level->tiles[tile_pos];
    UnitStack *stack = tile.stack;
    UnitStackView *stack_view;
    if (!stack) {
        draw_it = false;
    } else {
        stack_view = view->get_unit_stack_view(*stack);
        if (stack_view->moving)
            draw_it = false;
    }

    TileView& tile_view = view->level_view.tile_views[tile_pos];
    if (tile_view.highlighted) {
        Image *highlight1 = cursor_images[0].image;
        if (highlight1 != NULL)
            graphics->blit(highlight1, x - highlight1->width / 2, y - highlight1->height / 2 - 16, SDL_BLENDMODE_ADD, 128);
    }

    if (draw_it)
        draw_unit_stack(x, y, *stack_view);

    if (tile_view.highlighted && cursor_images.size() >= 2) {
        Image *highlight2 = cursor_images[1].image;
        if (highlight2 != NULL)
            graphics->blit(highlight2, x - highlight2->width / 2, y - highlight2->height / 2 - 16, SDL_BLENDMODE_ADD, 128);
    }
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
        TextFormat tf(graphics, SmallFont14, false, 255,255,255, 128,128,128);
        unit = tf.write_to_image(label);
        if (unit != NULL) {
            animation.images[(stack_view.phase / 1000) % animation.images.size()].image = unit;
        }
    }

    if (unit != NULL) {
        graphics->blit(unit, x - unit->width / 2, y - unit->height + 6, SDL_BLENDMODE_BLEND);
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
        graphics->blit(unit, x - unit->width / 2, y - unit->height + 6, SDL_BLENDMODE_ADD, add_phase);
    }

    Faction *owner = stack_view.stack->owner;
    FactionView *faction_view = view->faction_views[owner->id];
    FactionViewDef *faction_view_def = faction_view->view_def;

    graphics->fill_rectangle(faction_view_def->r, faction_view_def->g, faction_view_def->b, x+16, y-20, 8, 12);
}

void LevelRenderer::draw_unit(int x, int y, Unit &unit, UnitViewDef *view_def) {
    int facing = 2;
    std::vector<ImageRef>& image_series = view_def->hold_animations[facing].images;
    if (image_series.size() == 0)
        image_series.push_back(ImageRef("missing"));
    Image *image = image_series[0].image;
    if (image == NULL) {
        const std::string& label = view_def->name.substr(0, 3);
        TextFormat tf(graphics, SmallFont14, false, 255,255,255, 128,128,128);
        image = tf.write_to_image(label);
        image_series[0].image = image;
    }

    x -= image->clip_x_offset + image->clip_width / 2;
    y -= image->clip_y_offset + image->clip_height / 2;

    graphics->blit(image, x, y, SDL_BLENDMODE_BLEND);
}

void LevelRenderer::render_path_arrow(int x, int y, Point tile_pos) {
    TileView &tile_view = view->level_view.tile_views[tile_pos];

    switch (tile_view.path_dir) {
        case 0: y -= 16; break;
        case 1: x += 16; y -= 8; break;
        case 2: x += 16; y += 8; break;
        case 3: y += 16; break;
        case 4: x -= 16; y += 8; break;
        case 5: x -= 16; y -= 8; break;
    }

    if (tile_view.path_dir >= 0 && tile_view.path_dir < 6) {
        Image *path_arrow = arrow_images[tile_view.path_dir].image;

        if (path_arrow != NULL)
            graphics->blit(path_arrow, x - path_arrow->width / 2, y - path_arrow->height / 2 - 6, SDL_BLENDMODE_BLEND);
    }
}
