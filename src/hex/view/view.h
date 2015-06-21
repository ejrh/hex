#ifndef LEVEL_VIEW_H
#define LEVEL_VIEW_H

#include "hex/resources/resources.h"
#include "hex/resources/view_def.h"

class TileView {
public:
    TileView(): view_def(NULL), highlighted(false), path_dir(-1), variation(0), phase(0) { }
    ~TileView() { }

public:
    TileViewDef *view_def;
    bool highlighted;
    int path_dir;
    unsigned int variation;
    unsigned int phase;
};

class UnitStackView {
public:
    UnitStackView(): stack(NULL) { }
    UnitStackView(UnitStack *stack, UnitViewDef *view_def):
        stack(stack), view_def(view_def),
        facing(rand() % 6), variation(rand()), phase(rand()), selected(false), move_steps(0) { }
    ~UnitStackView() { }

public:
    UnitStack *stack;
    UnitViewDef *view_def;
    int facing;
    unsigned int variation;
    unsigned int phase;
    bool selected;
    Path path;
    int move_steps;
};

class MessageReceiver;

class LevelView {
public:
    LevelView(int width, int height, Resources *resources, MessageReceiver *dispatcher);
    ~LevelView();
    void update();
    void set_mouse_position(int x, int y);
    void mouse_to_tile(int x, int y, Point *tile);
    void tile_to_pixel(const Point tile, int *px, int *py);
    void shift(int xrel, int yrel);
    void left_click(int x, int y);
    void right_click(int x, int y);

public:
    boost::shared_ptr<Level> level;
    int width, height;
    std::map<int, UnitStackView> unit_stack_views;
    Vector2<TileView> tile_views;

protected:
    void set_drawn_path(Path& path);
    UnitStackView *get_unit_stack_view(const UnitStack &stack);
    TileView *get_tile_view(const Point tile_pos);

    void set_level(boost::shared_ptr<Level> level);

protected:
    Resources *resources;
    MessageReceiver *dispatcher;
    unsigned int last_update;
    Point highlight_tile;
    int shift_x, shift_y;
    int x_spacing, y_spacing;
    UnitStack *selected_stack;
    Path drawn_path;
    UnitStack *moving_unit;
    Path moving_unit_path;
    int moving_unit_progress;

    friend class LevelRenderer;
    friend class ViewUpdater;
};

class GameView {
public:
    GameView(int width, int height, Resources *resources, MessageReceiver *dispatcher): level_view(width, height, resources, dispatcher) { }

public:
    LevelView level_view;
};

#endif
