#include "common.h"

#include "hexutil/basics/hexgrid.h"

#include "hexgame/game/game.h"

#include "hexview/view/structure_painter.h"
#include "hexview/view/tile_painter.h"
#include "hexview/view/transition_paint.h"
#include "hexview/view/view.h"
#include "hexview/view/view_def.h"
#include "hexview/view/view_resources.h"


TilePainter::TilePainter(Game *game, GameView *view, ViewResources *resources):
        game(game), view(view), resources(resources),
        tile_paint_counter("paint.tile"), tile_paint_time("paint.tile.time"), tile_script_error_counter("paint.tile.error"),
        feature_paint_counter("paint.feature"), feature_paint_time("paint.feature.time"), feature_script_error_counter("paint.feature.error"),
        transition_paint_counter("paint.transition"), transition_paint_time("paint.transition.time"), transition_script_error_counter("paint.transition.error") {
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

        FeatureType& feature_type = *game->level.tiles[tile_pos].feature_type;
        FeatureViewDef::pointer feature_view_def = resources->get_feature_view_def(feature_type.name);
        tile_view.feature_view_def = feature_view_def;
    }

    StructurePainter structure_painter(game, view, resources);

    for (int i = offset.y - 1; i <= offset.y + 1; i++) {
        if (i < 0 || i >= game->level.height)
            continue;

        for (int j = offset.x - 1; j < offset.x + len + 1; j++) {
            if (j < 0 || j >= game->level.width)
                continue;

            Point tile_pos(j, i);
            TileView& tile_view = view->level_view.tile_views[tile_pos];
            Tile& tile = game->level.tiles[tile_pos];
            repaint_tile(tile_view, tile, tile_pos);
            repaint_transition(tile_view, tile, tile_pos);
            repaint_feature(tile_view, tile, tile_pos);
            if (tile.structure) {
                structure_painter.repaint(*tile_view.structure_view);
            }
        }
    }
}

void TilePainter::repaint_tile(TileView& tile_view, const Tile& tile, const Point tile_pos) {
    Timer paint_time(tile_paint_time);

    tile_view.tile_paint.clear();
    if (!tile_view.view_def)
        return;

    Script *script = tile_view.view_def->script.get();
    if (!script)
        return;

    // The properties order for a tile paint script is:
    //   - Variables
    //   - Tile view
    //   - Tile view def
    //   - Tile
    //   - Tile type
    PaintExecution execution(&resources->scripts, resources, &tile_view.tile_paint);
    //execution.add_properties(&tile_view.properties);
    //execution.add_properties(&tile_view.view_def->properties);
    //execution.add_properties(&tile.properties);
    if (tile.type)
        execution.add_properties(&tile.type->properties);

    // Initial variables set:
    //   - tile_type
    //   - neighbourX_type where neighbour in direction X exists
    //   - tile_variation

    execution.variables.set<Atom>(Atom("tile_type"), tile.type->name);

    for (int dir = 0; dir < 6; dir++) {
        Point neighbour_pos = get_neighbour(tile_pos, dir);
        if (!game->level.contains(neighbour_pos))
            continue;

        Tile& neighbour = game->level.tiles[neighbour_pos];
        if (!neighbour.type)
            continue;

        std::ostringstream ss;
        ss << boost::format("neighbour%d_type") % dir;
        execution.variables.set<Atom>(Atom(ss.str()), neighbour.type->name);
    }

    execution.variables.set<int>(Atom("tile_variation"), tile_view.variation);

    try {
        execution.run(script);
    } catch (ScriptError& err) {
        BOOST_LOG_TRIVIAL(error) << boost::format("Error in script %s: %s") % script->name % err.what();
        ++tile_script_error_counter;
    }

    ++tile_paint_counter;
}


void TilePainter::repaint_transition(TileView& tile_view, const Tile& tile, const Point tile_pos) {
    Timer paint_time(transition_paint_time);

    tile_view.transition_paint.clear();
    if (!tile_view.view_def)
        return;

    Script *script = tile_view.view_def->transition_script.get();
    if (!script)
        return;

    // The properties order for a transition paint script is:
    //   - Variables
    //   - Tile view
    //   - Tile view def
    //   - Tile
    //   - Tile type
    TransitionPaintExecution execution(&resources->scripts, resources, &tile_view.transition_paint, game, view, tile_pos);
    //execution.add_properties(&tile_view.properties);
    //execution.add_properties(&tile_view.view_def->properties);
    //execution.add_properties(&tile.properties);
    if (tile.type)
        execution.add_properties(&tile.type->properties);

    // Initial variables set:
    //   - tile_type
    //   - tile_variation

    execution.variables.set<Atom>(Atom("tile_type"), tile.type->name);

    execution.variables.set<int>(Atom("tile_variation"), tile_view.variation);

    try {
        execution.run(script);
    } catch (ScriptError& err) {
        BOOST_LOG_TRIVIAL(error) << boost::format("Error in script %s: %s") % script->name % err.what();
        ++transition_script_error_counter;
    }

    ++transition_paint_counter;
}


void TilePainter::repaint_feature(TileView& tile_view, const Tile& tile, const Point tile_pos) {
    Timer paint_time(feature_paint_time);

    tile_view.feature_paint.clear();
    if (!tile_view.feature_view_def)
        return;

    Script *script = tile_view.feature_view_def->script.get();
    if (!script)
        return;

    // The properties order for a feature paint script is:
    //   - Variables
    //   - Tile view
    //   - Tile view def
    //   - Tile
    //   - Tile type
    PaintExecution execution(&resources->scripts, resources, &tile_view.feature_paint);
    //execution.add_properties(&tile_view.properties);
    //execution.add_properties(&tile_view.view_def->properties);
    //execution.add_properties(&tile.properties);
    if (tile.type)
        execution.add_properties(&tile.type->properties);

    // Initial variables set:
    //   - tile_type
    //   - feature_type
    //   - tile_variation
    //   - neighbourX_road where neighbour in direction X exists

    execution.variables.set<Atom>(Atom("tile_type"), tile.type->name);
    execution.variables.set<Atom>(Atom("feature_type"), tile.feature_type->name);
    execution.variables.set<int>(Atom("tile_variation"), tile_view.variation);

    for (int dir = 0; dir < 6; dir++) {
        Point neighbour_pos = get_neighbour(tile_pos, dir);
        if (!game->level.contains(neighbour_pos))
            continue;

        Tile& neighbour = game->level.tiles[neighbour_pos];
        if (!neighbour.type)
            continue;

        std::ostringstream ss;
        ss << boost::format("neighbour%d_road") % dir;
        execution.variables.set<bool>(Atom(ss.str()), neighbour.has_property(Road));
    }

    try {
        execution.run(script);
    } catch (ScriptError& err) {
        BOOST_LOG_TRIVIAL(error) << boost::format("Error in script %s: %s") % script->name % err.what();
        ++feature_script_error_counter;
    }

    ++feature_paint_counter;
}
