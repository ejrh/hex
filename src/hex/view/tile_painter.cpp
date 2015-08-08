#include "common.h"

#include "hex/basics/hexgrid.h"
#include "hex/game/game.h"
#include "hex/resources/resources.h"
#include "hex/view/tile_painter.h"
#include "hex/view/view.h"


TilePainter::TilePainter(Game *game, GameView *view, Resources *resources):
        game(game), view(view), resources(resources) {
}

void TilePainter::repaint(const Point& tile_pos) {

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

    for (int i = 0; i < len; i++) {
        Point tile_pos(offset.x + i, offset.y);

        TileType *tile_type = game->level.tiles[tile_pos].type;
        TileView& tile_view = view->level_view.tile_views[tile_pos];
        TileViewDef *view_def = resources->get_tile_view_def(tile_type->name);
        tile_view.view_def = view_def;
        tile_view.variation = rand();
        tile_view.phase = rand();
    }

    for (int i = 0; i < len; i++) {
        Point tile_pos(offset.x + i, offset.y);

        TileView& tile_view = view->level_view.tile_views[tile_pos];
        TileViewDef *view_def = tile_view.view_def;
        Tile& tile = game->level.tiles[tile_pos];

        for (int j = 0; j < 3; j++) {
            int dir = (j + 5) % 6;
            Point neighbour;
            get_neighbour(tile_pos, dir, &neighbour);
            if (game->level.contains(neighbour)) {
                TileView& neighbour_view = view->level_view.tile_views[neighbour];
                Tile& neighbour_tile = game->level.tiles[neighbour];

                if (view_def != neighbour_view.view_def) {
                    tile_view.transition[j] = choose_image(view_def->transitions[j], tile_view.phase * (j + 2) / 1000);
                } else {
                    tile_view.transition[j] = NULL;
                }

                int opp_dir = (dir + 3) % 6;

                if (tile.road && neighbour_tile.road) {
                    tile_view.road[dir] = view_def->roads[dir].image;
                    neighbour_view.road[opp_dir] = neighbour_view.view_def->roads[opp_dir].image;
                } else {
                    tile_view.road[dir] = NULL;
                    neighbour_view.road[opp_dir] = NULL;
                }
            } else {
                tile_view.transition[j] = NULL;
                tile_view.road[dir] = NULL;
            }
        }
    }
}
