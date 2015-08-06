#include "common.h"

#include "hex/basics/error.h"
#include "hex/basics/hexgrid.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/game/movement.h"
#include "hex/game/pathfinding.h"
#include "hex/messaging/message.h"
#include "hex/resources/view_def.h"
#include "hex/view/player.h"
#include "hex/view/view.h"


LevelView::LevelView(Level *level):
        level(level), visibility(level), discovered(level), ghost_visibility(level) {
    resize(level->width, level->height);
}

LevelView::~LevelView() {
}

void LevelView::resize(int width, int height) {
    tile_views.resize(width, height);

    visibility.resize(width, height);
    discovered.resize(width, height);
    ghost_visibility.resize(width, height);
}

void LevelView::set_highlight_tile(const Point& tile_pos) {
    if (tile_views.contains(highlight_tile)) {
        tile_views[highlight_tile].highlighted = false;
    }

    highlight_tile = tile_pos;

    if (tile_views.contains(highlight_tile)) {
        tile_views[highlight_tile].highlighted = true;
    }
}

bool LevelView::check_visibility(const Point& tile_pos) {
    return visibility.check(tile_pos) || ghost_visibility.check(tile_pos);
}
