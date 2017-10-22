#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/basics/hexgrid.h"
#include "hexutil/messaging/message.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"
#include "hexgame/game/movement/movement.h"
#include "hexgame/game/movement/pathfinding.h"

#include "hexview/resources/view_def.h"
#include "hexview/view/player.h"
#include "hexview/view/view.h"


LevelView::LevelView(Level *level):
        level(level), visibility(*level), ghost_visibility(*level) {
    resize(level->width, level->height);
}

LevelView::~LevelView() {
}

void LevelView::resize(int width, int height) {
    tile_views.resize(width, height);
    for (int i = 0; i < tile_views.height; i++) {
        for (int j = 0; j < tile_views.width; j++) {
            tile_views[i][j].variation = rand();
        }
    }

    visibility.resize(width, height);
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

bool LevelView::check_discovered(const Point& tile_pos) {
    return discovered.check(tile_pos);
}

bool LevelView::check_visibility(const Point& tile_pos) {
    return visibility.check(tile_pos) || ghost_visibility.check(tile_pos);
}
