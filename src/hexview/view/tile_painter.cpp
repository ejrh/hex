#include "common.h"

#include "hexutil/basics/hexgrid.h"

#include "hexgame/game/game.h"

#include "hexview/resources/resources.h"
#include "hexview/view/tile_painter.h"
#include "hexview/view/view.h"


TilePainter::TilePainter(Game *game, GameView *view, Resources *resources):
        game(game), view(view), resources(resources) {
}

void TilePainter::repaint(Point offset, int len) {
    // Sanity check coordinates
    if (offset.y < 0 || offset.y >= game->level.height)
        return;
    if (offset.x < 0) {
        len += offset.x;
        offset.x = 0;
    }
    if (offset.x + len >= game->level.width) {
        len = game->level.width - offset.x;
    }

    // Set view defs
    for (int i = 0; i < len; i++) {
        Point tile_pos(offset.x + i, offset.y);

        TileType& tile_type = *game->level.tiles[tile_pos].type;
        TileView& tile_view = view->level_view.tile_views[tile_pos];
        TileViewDef::pointer view_def = resources->get_tile_view_def(tile_type.name);
        tile_view.view_def = view_def;
        tile_view.variation = rand();
        tile_view.phase = rand();
    }

    for (int i = offset.y - 1; i <= offset.y + 1; i++) {
        if (i < 0 || i >= game->level.height)
            continue;

        for (int j = offset.x - 1; j < offset.x + len + 1; j++) {
            if (j < 0 || j >= game->level.width)
                continue;

            Point tile_pos(j, i);
            paint_transitions(tile_pos);
            paint_roads(tile_pos);
            paint_features(tile_pos);
        }
    }
}

void TilePainter::paint_transitions(const Point& tile_pos) {
    TileView& tile_view = view->level_view.tile_views[tile_pos];

    tile_view.transitions.clear();

    TileViewDef::pointer view_def = tile_view.view_def;
    if (!view_def)
        return;

    for (auto iter = view_def->transitions.begin(); iter != view_def->transitions.end(); iter++) {
        TransitionDef& def = *iter;
        bool match = true;
        for (auto dir_iter = def.dirs.begin(); dir_iter != def.dirs.end(); dir_iter++) {
            Point neighbour_pos;
            get_neighbour(tile_pos, *dir_iter, &neighbour_pos);
            if (!game->level.contains(neighbour_pos)) {
                match = false;
                break;
            }
            TileViewDef::pointer neighbour_def = view->level_view.tile_views[neighbour_pos].view_def;
            if (!neighbour_def) {
                match = false;
                break;
            }
            if (def.type_names.count(neighbour_def->base_name) == 0) {
                match = false;
                break;
            }
        }

        if (match && def.images.size() > 0) {
            int r = rand() % def.images.size();
            tile_view.transitions.push_back(def.images[r].image);
        }
    }
}


void TilePainter::paint_roads(const Point& tile_pos) {
    TileView& tile_view = view->level_view.tile_views[tile_pos];

    tile_view.roads.clear();
    TileViewDef::pointer view_def = tile_view.view_def;
    if (!view_def)
        return;
    Tile& tile = game->level.tiles[tile_pos];
    if (!tile.has_property(Road))
        return;

    for (unsigned int dir = 0; dir < 6; dir++) {
        Point neighbour;
        get_neighbour(tile_pos, dir, &neighbour);
        if (!game->level.contains(neighbour))
            continue;

        Tile& neighbour_tile = game->level.tiles[neighbour];
        if (!neighbour_tile.has_property(Road))
            continue;

        if (view_def->roads.size() > dir)
            tile_view.roads.push_back(view_def->roads[dir].image);
    }
}

void TilePainter::paint_features(const Point& tile_pos) {
    TileView& tile_view = view->level_view.tile_views[tile_pos];
    tile_view.feature = NULL;
    TileViewDef::pointer view_def = tile_view.view_def;
    if (!view_def)
        return;

    for (auto iter = view_def->features.begin(); iter != view_def->features.end(); iter++) {
        tile_view.feature = choose_image(iter->images, tile_view.variation);
        tile_view.feature_x = iter->centre_x;
        tile_view.feature_y = iter->centre_y;
    }
}
