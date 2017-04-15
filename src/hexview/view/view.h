#ifndef LEVEL_VIEW_H
#define LEVEL_VIEW_H

#include "hexgame/game/game.h"

#include "hexview/resources/paint.h"
#include "hexview/resources/resources.h"
#include "hexview/resources/view_def.h"
#include "hexview/view/unit_painter.h"


class FactionView: public boost::enable_shared_from_this<FactionView> {
public:
    typedef boost::shared_ptr<FactionView> pointer;

    FactionView(Faction::pointer faction, FactionViewDef::pointer view_def): faction(faction), view_def(view_def) { }

public:
    Faction::pointer faction;
    FactionViewDef::pointer view_def;
};

class StructureView: public boost::enable_shared_from_this<StructureView> {
public:
    typedef boost::shared_ptr<StructureView> pointer;

    StructureView(Structure::pointer structure, StructureViewDef::pointer view_def):
            structure(structure), view_def(view_def), selected(false) { }

public:
    Structure::pointer structure;
    StructureViewDef::pointer view_def;
    Paint paint;
    bool selected;
};

class TileView {
public:
    TileView(): view_def(), highlighted(false), path_dir(-1), variation(0), feature(NULL), structure_view() { }
    ~TileView() { }

    static const int PATH_END = 8;

public:
    TileViewDef::pointer view_def;
    bool highlighted;
    int path_dir;
    unsigned int variation;
    std::vector<Image *> transitions;
    std::vector<Image *> roads;
    Image *feature;
    int feature_x, feature_y;
    StructureView::pointer structure_view;
};

enum UnitPosture {
    Holding,
    Moving,
    Attacking,
    Recoiling,
    Dying
};

class UnitView {
public:
    UnitView():
        facing(rand() % 6), posture(Holding), variation(rand()), phase(rand()), selected(false), targetted(false), play_sound(false) { }
    ~UnitView() { }

    void update(unsigned int update_ms) {
        phase += update_ms;
    }

public:
    UnitViewDef::pointer view_def;
    int facing;
    UnitPosture posture;
    unsigned int variation;
    unsigned int phase;
    bool selected;
    bool targetted;
    bool play_sound;
    Paint paint;
};

class UnitStackView: public UnitView, public boost::enable_shared_from_this<UnitStackView> {
public:
    typedef boost::shared_ptr<UnitStackView> pointer;

    UnitStackView(UnitStack::pointer stack):
        stack(stack),
        moving(false), locked(false) { }
    ~UnitStackView() { }

    void set_representative(Resources *resources);

public:
    UnitStack::pointer stack;
    int representative;
    bool moving, locked;
    Path path;
};

class GameView;
class UnitPainter;

class Ghost {
public:
    Ghost(GameView *view, UnitPainter *unit_painter, UnitStack::pointer& stack, const IntSet selected_units, Path path, UnitStack::pointer& target_stack);
    void update(unsigned int update_ms);

public:
    bool finished;
    GameView *view;

private:
    UnitPainter *unit_painter;
    UnitStackView::pointer target_view;
    UnitStackView::pointer stack_view;
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
    void left_click_tile(const Point& tile_pos);
    void right_click_tile(const Point& tile_pos);
    void set_drawn_path(const Point& start, const Path& path);
    Path find_path(UnitStack& party, const Point& from_pos, const Point& to_pos);
    void clear_drawn_path();
    void update_visibility();
    UnitStackView::pointer get_stack_view(int stack_id);
    TileView *get_tile_view(const Point tile_pos);
    void transfer_units(int stack_id, const IntSet selected_units, Path path, int target_id);
    void mark_ready();
    void set_view_def(UnitStackView& stack_view) const;
    void fix_stack_views();

public:
    Game *game;
    Player *player;
    std::vector<InfoMessage> messages;
    LevelView level_view;
    Resources *resources;
    UnitPainter unit_painter;
    MessageReceiver *dispatcher;
    unsigned int last_update;
    unsigned int phase;
    IntMap<FactionView> faction_views;
    IntMap<UnitStackView> unit_stack_views;
    std::vector<Ghost> ghosts;
    int selected_stack_id;
    IntSet selected_units;
    Structure::pointer selected_structure;
    Path drawn_path;

    bool debug_mode;

private:
    Counter ghost_counter;
};

#endif
