#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/basics/hexgrid.h"
#include "hexutil/basics/noise.h"

#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"

#include "hexgame/game/game.h"
#include "hexgame/game/movement/movement.h"
#include "hexgame/game/movement/pathfinding.h"

#include "hexview/view/level_renderer.h"
#include "hexview/view/level_window.h"
#include "hexview/view/view.h"


namespace hex {

#define TILE_WIDTH 48
#define TILE_HEIGHT 32
#define X_SPACING 32
#define Y_SPACING 32

#define SLOPE_WIDTH (TILE_WIDTH - X_SPACING)
#define SLOPE_HEIGHT (Y_SPACING/2)


static Point source(3,5);
static Point target(17,12);
static Point pivot_pos[10];  // 9 elements starting at 1

static ViewResources resources;

static TileType::pointer open_tile(boost::make_shared<TileType>());
static TileType::pointer forest_tile(boost::make_shared<TileType>());
static TileType::pointer closed_tile(boost::make_shared<TileType>());

UnitStack::pointer party = boost::make_shared<UnitStack>(0, source, Faction::pointer());

enum Mode {
    EditMode,
    StartMode,
    TargetMode,
    PivotMode,
    WeightMode
};

class PathfindingView: public GameView {
public:
    PathfindingView(Game *game):
            GameView(game, NULL, &hex::resources, NULL, NULL), movement_model(game), pathfinder(&game->level, &movement_model),
            brush_radius(2), pivot_num(1), weight(1), mode(EditMode) {
    }

    void reset() {
        for (int i = 0; i < level_view.tile_views.height; i++)
            for (int j = 0; j < level_view.tile_views.width; j++) {
                level_view.tile_views[i][j].variation = 0;
            }

        pathfinder.clear();

        MovementModel movement(game);
        for (int i = 1; i <= 9; i++) {
            pivot[i] = std::unique_ptr<Pivot>(calculate_pivot(pivot_pos[i], Walking, &game->level, &movement));
            pathfinder.add_pivot(pivot[i].get());
        }

        pathfinder.weight = weight;

        if (level_view.level->contains(source) && level_view.level->contains(target)) {
            pathfinder.start(*party, source, target);
        }

        pathfinder.complete();
    }

    void left_click() {
        switch (mode) {
            case EditMode: {
                Point tile_pos = level_view.highlight_tile;
                if (!level_view.level->contains(tile_pos))
                    return;

                TileType::pointer paint;
                if (level_view.level->tiles[tile_pos].type == closed_tile)
                    paint = open_tile;
                else if (level_view.level->tiles[tile_pos].type == open_tile)
                    paint = forest_tile;
                else
                    paint = closed_tile;

                std::vector<int> scanlines = get_circle_scanlines(tile_pos, brush_radius);
                for (int i = 0; i < scanlines.size(); i++) {
                    for (int j = -scanlines[i]; j <= scanlines[i]; j++) {
                        Point point(tile_pos.x + j, tile_pos.y - brush_radius + i);
                        if (level_view.level->contains(point))
                            level_view.level->tiles[point].type = paint;
                    }
                }
            } break;
            case PivotMode: {
                pivot_pos[pivot_num] = level_view.highlight_tile;
            } break;
            case StartMode: {
                source = level_view.highlight_tile;
            } break;
            case TargetMode: {
                target = level_view.highlight_tile;
            } break;
        };

        reset();
    }

    void update() {
        if (pathfinder.state == RUNNING) {
            pathfinder.step();
        } else if (pathfinder.state == FINISHED) {
            Path new_path;
            pathfinder.build_path(new_path);
            for (auto iter = new_path.begin(); iter != new_path.end(); iter++) {
                level_view.tile_views[iter->y][iter->x].variation = 1;
            }
        }
    }

    MovementModel movement_model;
    Pathfinder pathfinder;
    std::unique_ptr<Pivot> pivot[10];
    int brush_radius;
    int pivot_num;
    int weight;
    Mode mode;
};


class PathfindingRenderer: public LevelRenderer {
public:
    PathfindingRenderer(Graphics *graphics, Level *level, PathfindingView *view):
            LevelRenderer(graphics, &hex::resources, level, view, NULL),
            cache1(graphics, TextFormat(SmallFont10, true, 250,50,50), 1000),
            cache2(graphics, TextFormat(SmallFont10, true, 100,200,50), 1000),
            cache3(graphics, TextFormat(SmallFont10, true, 255,255,255), 10) { }
    void render_tile(int x, int y, Point tile_pos);

private:
    TextCache cache1;
    TextCache cache2;
    TextCache cache3;
};

void PathfindingRenderer::render_tile(int x, int y, Point tile_pos) {
    x -= TILE_WIDTH/2;
    y -= Y_SPACING/2;
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
    if (tile_pos == view->level_view.highlight_tile) {
        graphics->draw_lines(255,255,255, points, 7);
    } else {
        graphics->draw_lines(100,100,200, points, 7);
    }

    Tile &tile = level->tiles[tile_pos];
    TileView &tile_view = view->level_view.tile_views[tile_pos];

    if (tile.type == closed_tile) {
        SDL_Rect rect = { x + 12, y + 4, TILE_WIDTH - 24, TILE_HEIGHT - 8 };
        SDL_SetRenderDrawColor(graphics->renderer, 100,200,200, 255);
        SDL_RenderFillRect(graphics->renderer, &rect);
    } else if (tile.type == forest_tile) {
        SDL_Rect rect = { x + 12, y + 4, TILE_WIDTH - 24, TILE_HEIGHT - 8 };
        SDL_SetRenderDrawColor(graphics->renderer, 000,100,000, 255);
        SDL_RenderFillRect(graphics->renderer, &rect);
    }

    Pathfinder &pathfinder = (static_cast<PathfindingView *>(view))->pathfinder;
    PathfinderNode &node = pathfinder.nodes[tile_pos];

    if (node.state != PathfinderNode::State::None && node.cost != 0) {
        int x1, y1;
        int x2, y2;
        point_to_pixel(tile_pos, X_SPACING, Y_SPACING, &x1, &y1);
        point_to_pixel(node.predecessor, X_SPACING, Y_SPACING, &x2, &y2);
        x2 += (x - x1) + TILE_WIDTH/2;
        y2 += (y - y1) + TILE_HEIGHT/2;
        x1 = x + TILE_WIDTH/2;
        y1 = y + TILE_HEIGHT/2;
        SDL_SetRenderDrawColor(graphics->renderer, 200,200,200, 255);
        SDL_RenderDrawLine(graphics->renderer, x1, y1, x2, y2);
    }

    int tile_pivot_num = 0;
    for (int i = 1; i <= 9; i++)
        if (tile_pos == pivot_pos[i])
            tile_pivot_num = i;

    if (tile_pos == source) {
        SDL_Rect rect = { x + 16, y + 8, TILE_WIDTH - 32, TILE_HEIGHT - 16 };
        SDL_SetRenderDrawColor(graphics->renderer, 100,200,100, 255);
        SDL_RenderFillRect(graphics->renderer, &rect);
    } else if (tile_pos == target) {
        SDL_Rect rect = { x + 16, y + 8, TILE_WIDTH - 32, TILE_HEIGHT - 16 };
        SDL_SetRenderDrawColor(graphics->renderer, 200,100,100, 255);
        SDL_RenderFillRect(graphics->renderer, &rect);
    } else if (tile_pivot_num != 0) {
        SDL_Rect rect = { x + 16, y + 8, TILE_WIDTH - 32, TILE_HEIGHT - 16 };
        SDL_SetRenderDrawColor(graphics->renderer, 100,100,200, 255);
        SDL_RenderFillRect(graphics->renderer, &rect);
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "%d", tile_pivot_num);
        cache3.write_text(std::string(buffer), x + TILE_WIDTH/2, y + 16);
    } else if (tile_view.variation == 1) {
        SDL_Rect rect = { x + 16, y + 8, TILE_WIDTH - 32, TILE_HEIGHT - 16 };
        SDL_SetRenderDrawColor(graphics->renderer, 200,200,100, 255);
        SDL_RenderFillRect(graphics->renderer, &rect);
    } else {
        char buffer[50];
        snprintf(buffer, sizeof(buffer), "%d", node.cost);
        if (node.cost == INT_MAX) snprintf(buffer, sizeof(buffer), "-");
        cache1.write_text(std::string(buffer), x + TILE_WIDTH/2, y + 12);

        snprintf(buffer, sizeof(buffer), "%d", node.heuristic);
        if (node.heuristic == INT_MIN) snprintf(buffer, sizeof(buffer), "-");
        cache2.write_text(std::string(buffer), x + TILE_WIDTH/2, y + TILE_HEIGHT - 12);
    }
}


void generate_level(Level &level) {
    PerlinNoise noise(10, 10);
    PerlinNoise noise2(20, 20);

    for (int i = 0; i < level.height; i++) {
        for (int j = 0; j < level.width; j++) {
            float px = (float) j / level.width;
            float py = (float) i / level.height;
            if (j % 2 == 1)
                py += 0.5f / level.height;

            float value = (noise.value(px, py) + noise2.value(py, px))/6.0f;
            if (value < 0.0f)
                level.tiles[i][j].type = closed_tile;
            else if (value < 0.025f)
                level.tiles[i][j].type = forest_tile;
            else
                level.tiles[i][j].type = open_tile;
        }
    }
}


class BackgroundWindow: public UiWindow {
public:
    BackgroundWindow(UiLoop *loop, Graphics *graphics, PathfindingView *view):
        UiWindow(0, 0, graphics->width, graphics->height, WindowIsVisible|WindowIsActive|WindowWantsMouseEvents|WindowWantsKeyboardEvents),
        loop(loop), graphics(graphics), view(view) {
    }

    bool receive_mouse_event(SDL_Event *evt, int x, int y) {
        if (evt->type == SDL_MOUSEBUTTONUP && evt->button.button == SDL_BUTTON_LEFT) {
            if (!dragging) {
                view->left_click();
                return true;
            } else {
                dragging = false;
            }
        } else {
            if (evt->type == drag_event_type)
                dragging = true;

            return children[0]->receive_mouse_event(evt, x, y);
        }
        return false;
    }

    bool receive_keyboard_event(SDL_Event *evt) {
        if (evt->type == SDL_QUIT
            || (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE)) {
            loop->running = false;
            return true;
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym >= SDLK_1 && evt->key.keysym.sym <= SDLK_9) {
            int key_value = evt->key.keysym.sym - SDLK_0;
            if (view->mode == EditMode)
                view->brush_radius = key_value;
            else if (view->mode == PivotMode)
                view->pivot_num = key_value;
            else if (view->mode == WeightMode) {
                view->weight = key_value;
                view->reset();
            }
            return true;
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_e) {
            view->mode = EditMode;
            return true;
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_p) {
            view->mode = PivotMode;
            return true;
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_w) {
            view->mode = WeightMode;
            return true;
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_s) {
            view->mode = StartMode;
            return true;
        } else if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_t) {
            view->mode = TargetMode;
            return true;
        }
        return false;
    }

    void draw(const UiContext& context) {
        view->update();

        SDL_SetRenderDrawColor(graphics->renderer, 0,0,0, 255);
        SDL_RenderClear(graphics->renderer);
    }

private:
    UiLoop *loop;
    Graphics *graphics;
    PathfindingView *view;
    bool dragging;
};


class TopWindow: public UiWindow {
public:
    TopWindow(Graphics *graphics, PathfindingView *view):
        UiWindow(0, 0, 0, 0, WindowIsVisible),
        graphics(graphics), view(view) {
    }

    void draw(const UiContext& context) {
        TextFormat tf(SmallFont14, false, 255,255,255);
        auto s = boost::format("Mode: %s") % ((view->mode == EditMode) ? "Edit"
                : (view->mode == PivotMode) ? "Pivot"
                : (view->mode == StartMode) ? "Start"
                : (view->mode == TargetMode) ? "Target"
                : (view->mode == WeightMode) ? "Weight" : "?");
        tf.write_text(graphics, boost::str(s), 0, 0);

        s = boost::format("Weight: %d") % view->pathfinder.weight;
        tf.write_text(graphics, boost::str(s), 0, 20);

        s = boost::format("Path length: %d   Path cost: %d") % view->pathfinder.path_length % view->pathfinder.path_head.node->cost;
        tf.write_text(graphics, boost::str(s), 0, 40);

        s = boost::format("Nodes pushed: %d   Nodes popped: %d") % view->pathfinder.nodes_pushed % view->pathfinder.nodes_popped;
        tf.write_text(graphics, boost::str(s), 0, 60);

        s = boost::format("Time taken: %d") % view->pathfinder.time_taken;
        tf.write_text(graphics, boost::str(s), 0, 80);

        graphics->update();
    }

private:
    Graphics *graphics;
    PathfindingView *view;
};


void run() {
    register_property_names();

    open_tile->properties[Walkable] = 1;
    forest_tile->properties[Walkable] = 1;
    forest_tile->properties[SlowWalking] = 1;

    UnitType::pointer type = boost::make_shared<UnitType>();
    type->properties[Walking] = 1;
    Unit::pointer unit = boost::make_shared<Unit>();
    unit->type = type;
    party->units.push_back(unit);

    Graphics graphics("Pathfinding test", 0, 0, true);

    Game game;
    game.level.resize(200, 200);
    generate_level(game.level);

    PathfindingView view(&game);

    PathfindingRenderer level_renderer(&graphics, &game.level, &view);
    LevelWindow *level_window = new LevelWindow(graphics.width, graphics.height, &view, &level_renderer, NULL, &resources);

    UiLoop loop(&graphics, 25);
    BackgroundWindow *bg_window = new BackgroundWindow(&loop, &graphics, &view);
    loop.set_root_window(bg_window);
    bg_window->add_child(level_window);
    level_window->clear_flag(WindowIsActive);
    TopWindow *tw = new TopWindow(&graphics, &view);
    bg_window->add_child(tw);
    loop.run();
}

};


using namespace hex;

int main(int argc, char *argv[]) {
    try {
        run();
    } catch (Error &ex) {
        BOOST_LOG_TRIVIAL(fatal) << "Failed with: " << ex.what();
    }

    return 0;
}
