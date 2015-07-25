#include "common.h"

#include "hex/basics/error.h"
#include "hex/graphics/graphics.h"
#include "hex/graphics/font.h"
#include "hex/game/game.h"
#include "hex/view/view.h"
#include "hex/view/level_renderer.h"
#include "hex/view/level_window.h"
#include "hex/noise.h"
#include "hex/pathfinding.h"


#define TILE_WIDTH 48
#define TILE_HEIGHT 32
#define X_SPACING 32
#define Y_SPACING 32

#define SLOPE_WIDTH (TILE_WIDTH - X_SPACING)
#define SLOPE_HEIGHT (Y_SPACING/2)


static Point source(3,5);
static Point target(17,12);

static Resources resources;

static TileType open_tile("open", 1);
static TileType closed_tile("closed", -1);


class PathfindingLevelView: public LevelView {
public:
    PathfindingLevelView(Level *level):
        LevelView(level, &::resources, NULL), pathfinder(level) { }

    void left_click() {
        source = highlight_tile;
        if (level->contains(source) && level->contains(target)) {
            pathfinder.clear();
            pathfinder.start(source, target);
        }
    }

    void right_click() {
        target = highlight_tile;
        if (level->contains(source) && level->contains(target)) {
            pathfinder.clear();
            pathfinder.start(source, target);
        }
    }

    void middle_click() {
        Point tile_pos = highlight_tile;
        if (level->contains(tile_pos)) {
            if (level->tiles[tile_pos].type == &closed_tile)
                level->tiles[tile_pos].type = &open_tile;
            else
                level->tiles[tile_pos].type = &closed_tile;
        }
    }

    void update() {
        if (pathfinder.state == RUNNING) {
            pathfinder.step();

            if (pathfinder.state != CLEAR) {
                Path new_path;
                pathfinder.build_path(new_path);
                for (Path::const_iterator iter = new_path.begin(); iter != new_path.end(); iter++) {
                    tile_views[iter->y][iter->x].phase = 1;
                }
            }
        }
    }

    Pathfinder pathfinder;
};


class PathfindingRenderer: public LevelRenderer {
public:
    PathfindingRenderer(Graphics *graphics, Level *level, PathfindingLevelView *level_view):
            LevelRenderer(graphics, &::resources, level, level_view) { }
    void render_tile(int x, int y, Point tile_pos);
};

void PathfindingRenderer::render_tile(int x, int y, Point tile_pos) {
    int p1 = SLOPE_WIDTH;
    int p2 = TILE_WIDTH - SLOPE_WIDTH;
    int p3 = TILE_WIDTH;
    int p4 = SLOPE_HEIGHT;
    int p5 = TILE_HEIGHT;
    SDL_Point points[7] = { {p1,0}, {p2,0}, {p3,p4}, {p2,p5}, {p1,p5}, {0,p4}, {p1,0} };
    for (int i = 0; i < 7; i++) {
        points[i].x += x;
        points[i].y += y;
    }
    graphics->draw_lines(100,100,200, points, 7);

    Tile &tile = level->tiles[tile_pos];
    TileView &tile_view = level_view->tile_views[tile_pos];

    Pathfinder &pathfinder = (static_cast<PathfindingLevelView *>(level_view))->pathfinder;
    PathfinderNode &node = pathfinder.nodes[tile_pos];

    if (tile.type == &closed_tile) {
        SDL_Rect rect = { x + 16, y + 8, TILE_WIDTH - 32, TILE_HEIGHT - 16 };
        SDL_SetRenderDrawColor(graphics->renderer, 100,200,200, 255);
        SDL_RenderFillRect(graphics->renderer, &rect);
    } else if (tile_pos == source) {
        SDL_Rect rect = { x + 16, y + 8, TILE_WIDTH - 32, TILE_HEIGHT - 16 };
        SDL_SetRenderDrawColor(graphics->renderer, 100,200,100, 255);
        SDL_RenderFillRect(graphics->renderer, &rect);
    } else if (tile_pos == target) {
        SDL_Rect rect = { x + 16, y + 8, TILE_WIDTH - 32, TILE_HEIGHT - 16 };
        SDL_SetRenderDrawColor(graphics->renderer, 200,100,100, 255);
        SDL_RenderFillRect(graphics->renderer, &rect);
    } else if (tile_view.phase == 1) {
        SDL_Rect rect = { x + 16, y + 8, TILE_WIDTH - 32, TILE_HEIGHT - 16 };
        SDL_SetRenderDrawColor(graphics->renderer, 200,200,100, 255);
        SDL_RenderFillRect(graphics->renderer, &rect);
    } else {
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "%d", node.cost);
        if (node.cost == INT_MAX) snprintf(buffer, sizeof(buffer), "-");
        TextFormat tf1(graphics, SmallFont10, true, 250,50,50);
        tf1.write_text(std::string(buffer), x + TILE_WIDTH/2, y + 12);

        snprintf(buffer, sizeof(buffer), "%d", node.heuristic);
        if (node.heuristic == INT_MIN) snprintf(buffer, sizeof(buffer), "-");
        TextFormat tf2(graphics, SmallFont10, true, 100,200,50);
        tf2.write_text(std::string(buffer), x + TILE_WIDTH/2, y + TILE_HEIGHT - 12);
    }
}


void generate_level(Level &level) {
    PerlinNoise noise(5, 5);
    PerlinNoise noise2(10, 10);

    for (int i = 0; i < level.height; i++) {
        for (int j = 0; j < level.width; j++) {
            float px = (float) j / level.width;
            float py = (float) i / level.height;
            if (j % 2 == 1)
                py += 0.5f / level.height;

            float value = (noise.value(px, py) + noise2.value(py, px))/6.0f;
            if (value < 0.0f)
                level.tiles[i][j].type = &closed_tile;
            else
                level.tiles[i][j].type = &open_tile;
        }
    }
}


void run() {
    Graphics graphics;
    graphics.start();

    Level *level = new Level(200,200);
    generate_level(*level);

    PathfindingLevelView level_view(level);

    PathfindingRenderer level_renderer(&graphics, level, &level_view);
    LevelWindow level_window(graphics.width, graphics.height, &level_view, &level_renderer, &resources);

    int down_pos_x, down_pos_y;
    bool dragging = false;

    SDL_Event evt;
    bool running = true;
    while (running) {
        SDL_WaitEventTimeout(&evt, 500);
        if (evt.type == SDL_QUIT
            || (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE))
            running = false;

        if (evt.type == SDL_MOUSEMOTION) {
            level_window.set_mouse_position(evt.motion.x, evt.motion.y);
        }

        if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LMASK) {
            down_pos_x = evt.motion.x;
            down_pos_y = evt.motion.y;
        } else if (evt.type == SDL_MOUSEBUTTONUP && dragging) {
            dragging = false;
        } else if (evt.type == SDL_MOUSEBUTTONUP && evt.button.button == SDL_BUTTON_LEFT) {
            level_view.left_click();
        } else if (evt.type == SDL_MOUSEBUTTONUP && evt.button.button == SDL_BUTTON_MIDDLE) {
            level_view.right_click();
        } else if (evt.type == SDL_MOUSEBUTTONUP && evt.button.button == SDL_BUTTON_RIGHT) {
            level_view.middle_click();
        } else if (evt.type == SDL_MOUSEMOTION && dragging) {
            level_window.shift(evt.motion.xrel, evt.motion.yrel);
        } else if (evt.type == SDL_MOUSEMOTION) {
            if (evt.motion.state == SDL_BUTTON_LMASK &&  abs(evt.motion.x - down_pos_x) > 4 && abs(evt.motion.y - down_pos_y) > 4)
                dragging = true;
        }

        level_view.update();

        SDL_SetRenderDrawColor(graphics.renderer, 0,0,0, 255);
        SDL_RenderClear(graphics.renderer);

        level_window.draw();
        graphics.update();
    }

    delete level;

    graphics.stop();
}

int main(int argc, char *argv[]) {
    try {
        run();
    } catch (Error &ex) {
        std::cerr << "Failed with: " << ex.what() << std::endl;
    }

    return 0;
}
