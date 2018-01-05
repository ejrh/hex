#include "common.h"

#include "hexutil/basics/point.h"
#include "hexutil/basics/hexgrid.h"

#include "hexav/graphics/font.h"

#include "hexgame/game/game.h"

#include "hexview/view/audio_renderer.h"
#include "hexview/view/ghost.h"
#include "hexview/view/level_window.h"
#include "hexview/view/view.h"


namespace hex {

#define TILE_WIDTH 48
#define X_SPACING 32
#define Y_SPACING 32

#define SLOPE_WIDTH (TILE_WIDTH - X_SPACING)
#define SLOPE_HEIGHT (Y_SPACING/2)

LevelWindow::LevelWindow(int width, int height, GameView *view, LevelRenderer *level_renderer, AudioRenderer *audio_renderer, Resources *resources):
        UiWindow(0, 0, width, height, WindowIsVisible|WindowIsActive|WindowWantsMouseEvents|WindowWantsKeyboardEvents),
        view(view), level_renderer(level_renderer), audio_renderer(audio_renderer), terrain_only(false), resources(resources),
        shift_x(SLOPE_WIDTH), shift_y(SLOPE_HEIGHT), x_spacing(X_SPACING), y_spacing(Y_SPACING),
        scroll_up(false), scroll_down(false), scroll_left(false), scroll_right(false) {
    shift(0, 0);
}

LevelWindow::~LevelWindow() {
}

void LevelWindow::set_mouse_position(int x, int y) {
    Point tile_pos;

    mouse_to_tile(x, y, &tile_pos);
    view->level_view.set_highlight_tile(tile_pos);
}

void LevelWindow::mouse_to_tile(int x, int y, Point *tile) {
    x += shift_x - this->x;
    y += shift_y - this->y;
    *tile = pixel_to_point(x, y, X_SPACING, Y_SPACING, SLOPE_WIDTH, SLOPE_HEIGHT);
}

void LevelWindow::tile_to_pixel(const Point tile, int *px, int *py) {
    *px = tile.x * x_spacing - shift_x + x;
    *py = tile.y * y_spacing - shift_y + y;
    if (tile.x % 2 == 1)
        *py += y_spacing / 2;
}

void LevelWindow::set_position(int x, int y) {
    int min_shift_x = SLOPE_WIDTH;
    int max_shift_x = view->level_view.tile_views.width * X_SPACING - width;
    int min_shift_y = SLOPE_HEIGHT;
    int max_shift_y = view->level_view.tile_views.height * Y_SPACING - height;

    shift_x = x;
    if (shift_x < min_shift_x)
        shift_x = min_shift_x;
    if (shift_x > max_shift_x)
        shift_x = max_shift_x;

    shift_y = y;
    if (shift_y < min_shift_y)
        shift_y = min_shift_y;
    if (shift_y > max_shift_y)
        shift_y = max_shift_y;

    if (min_shift_x > max_shift_x)
        shift_x = (min_shift_x + max_shift_x) / 2;
    if (min_shift_y > max_shift_y)
        shift_y = (min_shift_y + max_shift_y) / 2;
}

void LevelWindow::shift(int xrel, int yrel) {
    set_position(shift_x - xrel, shift_y - yrel);
}

void LevelWindow::left_click(int x, int y) {
    Point tile_pos;

    mouse_to_tile(x, y, &tile_pos);
    if (view->level_view.level->contains(tile_pos))
        view->left_click_tile(tile_pos);
}

void LevelWindow::right_click(int x, int y) {
    Point tile_pos;

    mouse_to_tile(x, y, &tile_pos);
    if (view->level_view.level->contains(tile_pos))
        view->right_click_tile(tile_pos);
}

bool LevelWindow::receive_mouse_event(SDL_Event *evt, int x, int y) {
    if (evt->type == SDL_MOUSEMOTION) {
        set_mouse_position(evt->motion.x, evt->motion.y);
        return true;
    } else if (evt->type == SDL_MOUSEBUTTONDOWN && evt->button.button == SDL_BUTTON_LEFT) {
        return true;
    } else if (evt->type == SDL_MOUSEBUTTONUP && evt->button.button == SDL_BUTTON_LEFT) {
        left_click(evt->button.x, evt->button.y);
        return true;
    } else if (evt->type == SDL_MOUSEBUTTONUP && evt->button.button == SDL_BUTTON_RIGHT) {
        right_click(evt->button.x, evt->button.y);
        return true;
    } else if (evt->type == drag_event_type) {
        shift(evt->motion.xrel, evt->motion.yrel);
    }

    return false;
}

bool LevelWindow::receive_keyboard_event(SDL_Event *evt) {
    if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_UP) {
        scroll_up = true;
    } else if (evt->type == SDL_KEYUP && evt->key.keysym.sym == SDLK_UP) {
        scroll_up = false;
    } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_DOWN) {
        scroll_down = true;
    } else if (evt->type == SDL_KEYUP && evt->key.keysym.sym == SDLK_DOWN) {
        scroll_down = false;
    } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_LEFT) {
        scroll_left = true;
    } else if (evt->type == SDL_KEYUP && evt->key.keysym.sym == SDLK_LEFT) {
        scroll_left = false;
    } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_RIGHT) {
        scroll_right = true;
    } else if (evt->type == SDL_KEYUP && evt->key.keysym.sym == SDLK_RIGHT) {
        scroll_right = false;
    }

    return false;
}

void LevelWindow::draw(const UiContext& context) {
    int scroll_x = 0;
    int scroll_y = 0;
    if (scroll_up) {
        scroll_y += 20;
    }
    if (scroll_down) {
        scroll_y -= 20;
    }
    if (scroll_left) {
        scroll_x += 20;
    }
    if (scroll_right) {
        scroll_x -= 20;
    }
    if (scroll_x != 0 || scroll_y != 0)
        shift(scroll_x, scroll_y);

    level_renderer->clear(x, y, width, height);
    draw_level(&LevelRenderer::render_tile);
    draw_level(&LevelRenderer::render_tile_transitions);
    draw_level(&LevelRenderer::render_features);
    if (!terrain_only) {
        draw_level(&LevelRenderer::render_objects);
        draw_level(&LevelRenderer::render_path_arrow);
        for (auto iter = view->ghosts.begin(); iter != view->ghosts.end(); iter++) {
            draw_ghost(*iter->second);
        }
        if (!view->debug_mode)
            draw_level(&LevelRenderer::render_fog);
    }

    if (view->debug_mode) {
        draw_info(context);
    }
}

void LevelWindow::draw_info(const UiContext& context) {
    context.graphics->fill_rectangle(0, 0, 0, x, y, 160, 40);
    if (!view->game->level.contains(view->level_view.highlight_tile))
        return;

    TextFormat tf;
    std::ostringstream ss1;
    ss1 << "Position: " << view->level_view.highlight_tile;
    tf.write_text(context.graphics, ss1.str(), x+4, y+4);
    Tile& tile = view->level_view.level->tiles[view->level_view.highlight_tile];

    std::ostringstream ss2;
    ss2 << "Type: " << tile.type->name;
    tf.write_text(context.graphics, ss2.str(), x+4, y+12);

    std::ostringstream ss3;
    ss3 << "Feature type: " << tile.feature_type->name;
    tf.write_text(context.graphics, ss3.str(), x+4, y+20);

    if (tile.structure) {
        std::ostringstream ss4;
        ss4 << "Structure type: " << tile.structure->type->name;
        tf.write_text(context.graphics, ss4.str(), x+4, y+28);
    }
}

void LevelWindow::draw_level(LevelRenderer::RenderMethod render) {
    Point min_pos, max_pos;
    mouse_to_tile(x, y, &min_pos);
    mouse_to_tile(x + width, y + height, &max_pos);

    min_pos.x -= 2;
    if (min_pos.x % 2 == 1)
        min_pos.x++;
    if (min_pos.x < 0)
        min_pos.x = 0;
    min_pos.y--;
    if (min_pos.y < 0)
        min_pos.y = 0;
    max_pos.x += 2;
    if (max_pos.x > view->level_view.tile_views.width)
        max_pos.x = view->level_view.tile_views.width;
    max_pos.y += 2;
    if (max_pos.y > view->level_view.tile_views.height)
        max_pos.y = view->level_view.tile_views.height;

    for (int i = min_pos.y; i < max_pos.y; i++) {
        for (int j = min_pos.x; j < max_pos.x; j += 2) {
            int xpos = j*x_spacing - shift_x + TILE_WIDTH / 2 + x;
            int ypos = i*y_spacing - shift_y + Y_SPACING / 2 + y;
            Point tile_pos(j, i);
            (*level_renderer.*render)(xpos, ypos, tile_pos);
        }

        for (int j = min_pos.x + 1; j < max_pos.x; j += 2) {
            int y_offset = y_spacing / 2;
            int xpos = j*x_spacing - shift_x + TILE_WIDTH / 2 + x;
            int ypos = i*y_spacing + y_offset - shift_y + Y_SPACING / 2 + y;
            Point tile_pos(j, i);
            (*level_renderer.*render)(xpos, ypos, tile_pos);
        }
    }
}

void LevelWindow::draw_ghost(Ghost& ghost) {
    UnitStackView::pointer& stack_view = ghost.stack_view;

    Point prev_pos = ghost.position;
    Point next_pos = ghost.target_position;

    if (!view->debug_mode && !view->level_view.check_visibility(prev_pos) && !view->level_view.check_visibility(next_pos))
        return;

    int px1, py1, px2, py2;
    tile_to_pixel(prev_pos, &px1, &py1);
    tile_to_pixel(next_pos, &px2, &py2);

    if (px1 < -X_SPACING || py1 < -Y_SPACING || px2 > width+X_SPACING || py2 > width+Y_SPACING)
        return;

    int px = (px1 * (1000 - ghost.progress) + px2 * ghost.progress) / 1000;
    int py = (py1 * (1000 - ghost.progress) + py2 * ghost.progress) / 1000;
    bool selected = stack_view->selected;
    stack_view->selected = false;
    level_renderer->draw_unit_stack(px + TILE_WIDTH / 2, py + Y_SPACING / 2, *stack_view);
    stack_view->selected = selected;

    audio_renderer->render_unit_sound(*stack_view);
}

};
