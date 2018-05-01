#ifndef BRUSH_H
#define BRUSH_H

#include "hexutil/basics/atom.h"
#include "hexutil/basics/point.h"

#include "hexview/view/view.h"


namespace hex {

class Brush {
public:
    virtual ~Brush() { }
    virtual void paint(const Point point, int radius, Game *game, GameView *view) = 0;
};


class TileTypeBrush: public Brush {
public:
    TileTypeBrush(TileType::pointer tile_type): tile_type(tile_type) { }
    void paint(const Point point, int radius, Game *game, GameView *view);

private:
    bool paint_tile(const Tile& tile, Atom& new_tile_type, Atom& new_feature_type);

public:
    TileType::pointer tile_type;
};


class FeatureTypeBrush: public Brush {
public:
    FeatureTypeBrush(FeatureType::pointer feature_type): feature_type(feature_type) { }
    void paint(const Point point, int radius, Game *game, GameView *view);

private:
    bool paint_tile(const Tile& tile, Atom& new_feature_type);

public:
    FeatureType::pointer feature_type;
};


class FeatureType;
class StructureType;

bool compatible(const Atom tile_type, const FeatureType& feature_type);
bool compatible(const Atom tile_type, const Atom feature_type, const StructureType& structure_type);

};

#endif

