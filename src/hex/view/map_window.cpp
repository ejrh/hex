#include "common.h"

#include "hex/game/game.h"
#include "hex/graphics/graphics.h"
#include "hex/view/view.h"
#include "hex/view/level_renderer.h"
#include "hex/view/level_window.h"
#include "hex/view/map_window.h"
#include "hex/view/unit_renderer.h"


MapWindow::MapWindow(int x, int y, int width, int height, GameView *view, LevelWindow *level_window, Graphics *graphics, Resources *resources):
        UiWindow(x, y, width, height, WindowIsVisible|WindowIsActive|WindowWantsMouseEvents|WindowWantsKeyboardEvents),
        view(view), level_window(level_window), graphics(graphics), resources(resources),
        dragging(false), map_image(NULL) {
}

MapWindow::~MapWindow() {
    if (map_image)
        delete map_image;
}

void MapWindow::mouse_to_tile(int x, int y, Point *tile) {
}

void MapWindow::tile_to_pixel(const Point tile, int *px, int *py) {
    *px = x + 4 + tile.x*4;
    *py = y + 4 + tile.y*4;
    if (tile.x % 2 == 1)
        *py += 2;
}

void MapWindow::left_click(int x, int y) {
    int px = x - this->x - 4;
    int py = y - this->y - 4;

    int lx = px / 4 * 32 - level_window->width / 2;
    int ly = py / 4 * 32 - level_window->height / 2;
    level_window->set_position(lx, ly);
}

void MapWindow::right_click(int x, int y) {
}

bool MapWindow::receive_mouse_event(SDL_Event *evt, int x, int y) {
    if (evt->type == SDL_MOUSEBUTTONDOWN && evt->button.button == SDL_BUTTON_LEFT) {
        left_click(evt->button.x, evt->button.y);
        return true;
    } else if (evt->type == drag_event_type) {
        left_click(evt->motion.x, evt->motion.y);
        return true;
    }

    return false;
}

bool MapWindow::receive_keyboard_event(SDL_Event *evt) {
    if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_F9) {
        create_map_image();
        return true;
    }

    return false;
}

void MapWindow::draw(const UiContext& context) {
    graphics->fill_rectangle(75,75,50, x, y, width, height);

    if (map_image) {
        SDL_Rect dest_rect = { x+4, y+4, width-8, height-8 };
        SDL_RenderCopy(graphics->renderer, map_image->texture, NULL, &dest_rect);

        for (int i = 0; i < view->level_view.tile_views.height; i++)
            for (int j = 0; j < view->level_view.tile_views.width; j++) {
                if (!view->debug_mode && !view->level_view.discovered.check(Point(j, i))) {
                    int px, py;
                    tile_to_pixel(Point(j, i), &px, &py);

                    graphics->fill_rectangle(75,75,50, px, py, 4, 4);
                }
            }
    } else {
        for (int i = 0; i < view->level_view.tile_views.height; i++)
            for (int j = 0; j < view->level_view.tile_views.width; j++) {
                if (!view->debug_mode && !view->level_view.discovered.check(Point(j, i)))
                    continue;

                TileView& tile_view = view->level_view.tile_views[i][j];
                if (!tile_view.view_def)
                    continue;

                int r = tile_view.view_def->r;
                int g = tile_view.view_def->g;
                int b = tile_view.view_def->b;

                int px, py;
                tile_to_pixel(Point(j, i), &px, &py);

                graphics->fill_rectangle(r,g,b, px, py, 4, 4);
            }
    }

    for (int i = 0; i < view->level_view.tile_views.height; i++)
        for (int j = 0; j < view->level_view.tile_views.width; j++) {
            if (!view->debug_mode && !view->level_view.discovered.check(Point(j, i)))
                continue;

            if (!view->debug_mode && !view->level_view.check_visibility(Point(j, i))) {
                int px, py;
                tile_to_pixel(Point(j, i), &px, &py);

                graphics->fill_rectangle(0, 0, 0, px+1, py+1, 2, 2);
            }

            TileView& tile_view = view->level_view.tile_views[i][j];
            if (!tile_view.structure_view)
                continue;

            int r, g, b;
            Faction::pointer& owner = tile_view.structure_view->structure->owner;
            if (owner) {
                FactionView::pointer faction_view = view->faction_views.get(owner->id);
                FactionViewDef::pointer faction_view_def = faction_view->view_def;
                r = faction_view_def->r;
                g = faction_view_def->g;
                b = faction_view_def->b;
            } else {
                r = g = b = 128;
            }

            int px, py;
            tile_to_pixel(Point(j, i), &px, &py);

            graphics->fill_rectangle(r,g,b, px, py, 4, 4);
        }

    for (IntMap<UnitStackView>::iterator iter = view->unit_stack_views.begin(); iter != view->unit_stack_views.end(); iter++) {
        UnitStack::pointer stack = iter->second->stack;
        if (iter->second->moving || (!view->debug_mode && !view->level_view.check_visibility(stack->position)))
            continue;

        int px, py;
        tile_to_pixel(stack->position, &px, &py);

        Faction::pointer owner = stack->owner;
        FactionView::pointer faction_view = view->faction_views.get(owner->id);
        FactionViewDef::pointer faction_view_def = faction_view->view_def;

        graphics->fill_rectangle(faction_view_def->r, faction_view_def->g, faction_view_def->b, px, py, 4, 4);
    }

    for (std::vector<Ghost>::iterator iter = view->ghosts.begin(); iter != view->ghosts.end(); iter++) {
        Ghost& ghost = *iter;
        UnitStack::pointer& stack = ghost.stack_view->stack;
        if (!view->level_view.check_visibility(stack->position))
            continue;

        int px, py;
        tile_to_pixel(stack->position, &px, &py);

        Faction::pointer owner = stack->owner;
        FactionView::pointer faction_view = view->faction_views.get(owner->id);
        FactionViewDef::pointer faction_view_def = faction_view->view_def;

        graphics->fill_rectangle(faction_view_def->r, faction_view_def->g, faction_view_def->b, px, py, 4, 4);
    }

    int px = this->x + 4 * level_window->shift_x / 32 + 4;
    int py = this->y + 4 * level_window->shift_y / 32 + 4;
    int w = 4 * level_window->width / 32;
    int h = 4 * level_window->height / 32;
    graphics->draw_rectangle(255,255,255, px, py, w, h);
}

void MapWindow::create_map_image() {
    int total_width = view->level_view.tile_views.width * 32;
    int total_height = view->level_view.tile_views.height * 32;
    LevelRenderer lr(graphics, view->resources, &view->game->level, view, NULL);
    LevelWindow lw(total_width, total_height, view, &lr, view->resources);
    lw.terrain_only = true;

    if (!map_image) {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        SDL_Texture *target_texture = SDL_CreateTexture(graphics->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, total_width, total_height);
        if (!target_texture) {
            BOOST_LOG_TRIVIAL(error) << "SDL error while creating target texture: " << SDL_GetError();
            return;
        }
        map_image = new Image(-1, target_texture);
    }

    graphics->set_target_image(map_image);
    UiContext context(graphics, 0, 0);
    lw.draw(context);
    graphics->unset_target_image();
}
