#include "common.h"

#include "hex/basics/error.h"
#include "hex/basics/vector2.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"
#include "hex/game/game.h"
#include "hex/view/level_renderer.h"
#include "hex/view/unit_renderer.h"
#include "hex/view/view.h"


LevelRenderer::LevelRenderer(Graphics *graphics, Resources *resources, Level *level, GameView *view, UnitRenderer *unit_renderer):
        show_hexagons(false), graphics(graphics), resources(resources), level(level), view(view), unit_renderer(unit_renderer) {
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
    TileViewDef::pointer def = tile_view.view_def;
    if (!def)
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
            int trans_y = y - trans->height / 2;
            graphics->blit(trans, trans_x, trans_y, SDL_BLENDMODE_BLEND, alpha);
        }
    }
}

void LevelRenderer::render_features(int x, int y, Point tile_pos) {
    TileView& tile_view = view->level_view.tile_views[tile_pos];

    if (tile_view.feature != NULL) {
        int alpha = (view->level_view.check_visibility(tile_pos)) ? 255 : 128;
        int feature_x = x - tile_view.feature_x;
        int feature_y = y - tile_view.feature_y;
        graphics->blit(tile_view.feature, feature_x, feature_y, SDL_BLENDMODE_BLEND, alpha);
    }

    for (std::vector<Image *>::iterator iter = tile_view.roads.begin(); iter != tile_view.roads.end(); iter++) {
        Image *road = *iter;
        if (road != NULL) {
            int alpha = (view->level_view.check_visibility(tile_pos)) ? 255 : 128;
            graphics->blit(road, x - road->width / 2, y - road->height / 2, SDL_BLENDMODE_BLEND, alpha);
        }
    }
}

#define TILE_WIDTH 48
#define TILE_HEIGHT 32
#define X_SPACING 32
#define Y_SPACING 32
#define SLOPE_WIDTH (TILE_WIDTH - X_SPACING)
#define SLOPE_HEIGHT (Y_SPACING/2)

void LevelRenderer::render_objects(int x, int y, Point tile_pos) {
    TileView& tile_view = view->level_view.tile_views[tile_pos];

    if (tile_view.structure_view) {
        StructureViewDef::pointer view_def = tile_view.structure_view->view_def;
        AnimationDef& animation = view_def->animation;
        Image *structure = unit_renderer->get_image_or_placeholder(animation.images, tile_view.phase / 1000, view_def->name);

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

        Faction::pointer& owner = tile_view.structure_view->structure->owner;
        if (owner) {
            FactionView::pointer faction_view = view->faction_views.get(owner->id);
            FactionViewDef::pointer faction_view_def = faction_view->view_def;
            graphics->fill_rectangle(faction_view_def->r, faction_view_def->g, faction_view_def->b, x+16, y-20, 8, 12);
        }
    }

    if (show_hexagons) {
        int p1 = SLOPE_WIDTH;
        int p2 = TILE_WIDTH - SLOPE_WIDTH;
        int p3 = TILE_WIDTH;
        int p4 = SLOPE_HEIGHT;
        int p5 = TILE_HEIGHT;
        SDL_Point points[7] = { {p1,0}, {p2,0}, {p3,p4}, {p2,p5}, {p1,p5}, {0,p4}, {p1,0} };
        for (int i = 0; i < 7; i++) {
            points[i].x += x - TILE_WIDTH/2;
            points[i].y += y - TILE_HEIGHT/2;
        }
        graphics->draw_lines(100,100,200, points, 7);
    }

    bool draw_it = true;
    if (!view->level_view.check_visibility(tile_pos))
        return;

    Tile &tile = level->tiles[tile_pos];
    UnitStack::pointer stack = tile.stack;
    UnitStackView::pointer stack_view;
    if (!stack) {
        draw_it = false;
    } else {
        stack_view = view->get_stack_view(stack->id);
        if (stack_view->moving)
            draw_it = false;
    }

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
    unit_renderer->draw_unit(x, y, stack_view);

    // Draw flag
    Faction::pointer owner = stack_view.stack->owner;
    FactionView::pointer faction_view = view->faction_views.get(owner->id);
    FactionViewDef::pointer faction_view_def = faction_view->view_def;

    graphics->fill_rectangle(faction_view_def->r, faction_view_def->g, faction_view_def->b, x+16, y-20, 8, 12);
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
