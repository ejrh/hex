#ifndef LEVEL_VIEW_H
#define LEVEL_VIEW_H

#include "hex/resources/resources.h"
#include "hex/resources/view_def.h"

class FactionView {
public:
    FactionView(Faction *faction, FactionViewDef *view_def): faction(faction), view_def(view_def) { }

public:
    Faction *faction;
    FactionViewDef *view_def;
};

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
        facing(rand() % 6), variation(rand()), phase(rand()), selected(false), moving(false), move_steps(0) { }
    ~UnitStackView() { }

public:
    UnitStack *stack;
    UnitViewDef *view_def;
    int facing;
    unsigned int variation;
    unsigned int phase;
    bool selected;
    bool moving;
    Path path;
    int move_steps;
};

class MessageReceiver;

class LevelView {
public:
    LevelView(Level *level, Resources *resources, MessageReceiver *dispatcher);
    ~LevelView();
    void resize(int width, int height);
    void update();
    void set_highlight_tile(const Point& tile_pos);
    void left_click_tile(const Point& tile_pos);
    void right_click_tile(const Point& tile_pos);

public:
    Level *level;
    std::map<int, UnitStackView> unit_stack_views;
    Vector2<TileView> tile_views;

protected:
    void set_drawn_path(Path& path);
    UnitStackView *get_unit_stack_view(const UnitStack &stack);
    TileView *get_tile_view(const Point tile_pos);

protected:
    Resources *resources;
    MessageReceiver *dispatcher;
    unsigned int last_update;
    Point highlight_tile;
    UnitStack *selected_stack;
    VisibilityMap visibility;
    VisibilityMap discovered;
    Path drawn_path;
    UnitStack *moving_unit;
    Path moving_unit_path;
    int moving_unit_progress;

    friend class LevelRenderer;
    friend class LevelWindow;
    friend class ViewUpdater;
};

class GameView {
public:
    GameView(Game *game, Resources *resources, MessageReceiver *dispatcher): game(game), level_view(&game->level, resources, dispatcher) { }

public:
    Game *game;
    LevelView level_view;
    std::map<int, FactionView *> faction_views;
};

#endif
