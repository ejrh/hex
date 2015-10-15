#ifndef LEVEL_VIEW_H
#define LEVEL_VIEW_H

#include "hex/game/game.h"
#include "hex/resources/resources.h"
#include "hex/resources/view_def.h"


class FactionView {
public:
    FactionView(Faction *faction, FactionViewDef *view_def): faction(faction), view_def(view_def) { }

public:
    Faction *faction;
    FactionViewDef *view_def;
};

class StructureView {
public:
    StructureView(Structure *structure, StructureViewDef *view_def):
      structure(structure), view_def(view_def), selected(false) { }

public:
    Structure *structure;
    StructureViewDef *view_def;
    bool selected;
};

class TileView {
public:
    TileView(): view_def(NULL), highlighted(false), path_dir(-1), variation(0), phase(0), feature(NULL), structure_view(NULL) { }
    ~TileView() { }

public:
    TileViewDef *view_def;
    bool highlighted;
    int path_dir;
    unsigned int variation;
    unsigned int phase;
    std::vector<Image *> transitions;
    std::vector<Image *> roads;
    Image *feature;
    int feature_x, feature_y;
    StructureView *structure_view;
};

class UnitStackView {
public:
    UnitStackView(): stack(NULL) { }
    UnitStackView(UnitStack *stack, UnitViewDef *view_def):
        stack(stack), view_def(view_def),
        facing(rand() % 6), variation(rand()), phase(rand()), selected(false), moving(false), locked(false), move_steps(0) { }
    ~UnitStackView() { }

public:
    UnitStack *stack;
    UnitViewDef *view_def;
    int facing;
    unsigned int variation;
    unsigned int phase;
    bool selected;
    bool moving, locked;
    Path path;
    int move_steps;
};

class Ghost {
public:
    Ghost(int target_id, Point position, Path path);

private:
    int target_id;
    Point position;
    Path path;
    unsigned int step;
    unsigned int progress;

    friend class GameView;
    friend class LevelView;
    friend class LevelWindow;
    friend class MapWindow;
    friend class ViewUpdater;
};

class MessageReceiver;
class Player;

class LevelView {
public:
    LevelView(Level *level);
    ~LevelView();

    void resize(int width, int height);
    void set_highlight_tile(const Point& tile_pos);
    bool check_visibility(const Point& tile_pos);

public:
    Level *level;
    Vector2<TileView> tile_views;
    Point highlight_tile;
    VisibilityMap visibility;
    VisibilityMap discovered;
    VisibilityMap ghost_visibility;
};


class InfoMessage {
public:
    InfoMessage(const std::string text): text(text) { }
public:
    std::string text;
};

class GameView {
public:
    GameView(Game *game, Player *player, Resources *resources, MessageReceiver *dispatcher);

    void update();
    bool update_ghost(Ghost& ghost, unsigned int update_ms);
    void left_click_tile(const Point& tile_pos);
    void right_click_tile(const Point& tile_pos);
    void set_drawn_path(const Point& start, const Path& path);
    void clear_drawn_path();
    void update_visibility();
    UnitStackView *get_stack_view(int stack_id);
    TileView *get_tile_view(const Point tile_pos);
    void transfer_units(int stack_id, const IntSet selected_units, Path path, int target_id);
    void mark_ready();

public:
    Game *game;
    Player *player;
    std::vector<InfoMessage> messages;
    LevelView level_view;
    Resources *resources;
    MessageReceiver *dispatcher;
    unsigned int last_update;
    unsigned int phase;
    std::map<int, FactionView *> faction_views;
    std::map<int, UnitStackView> unit_stack_views;
    std::vector<Ghost> ghosts;
    int selected_stack_id;
    IntSet selected_units;
    Structure *selected_structure;
    Path drawn_path;

    bool debug_mode;
};

#endif
