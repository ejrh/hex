#ifndef VIEW_DATA_H
#define VIEW_DATA_H

#include "hex/graphics/graphics.h"
#include "hex/resources/image_ref.h"


class FactionViewDef {
public:
    FactionViewDef() { }
    FactionViewDef(std::string name, int r, int g, int b): name(name), r(r), g(g), b(b) { }

    std::string name;
    int r, g, b;
};

typedef std::map<std::string, FactionViewDef *> FactionViewDefMap;

typedef std::vector<ImageSeries> TileImageMap;

class TileViewDef {
public:
    TileViewDef() { }
    TileViewDef(std::string name): name(name) { }
    ~TileViewDef() { }

    std::string name;
    TileImageMap images;
};

typedef std::map<std::string, TileViewDef *> TileViewDefMap;


typedef std::vector<ImageSeries> UnitImageMap;

class UnitViewDef {
public:
    UnitViewDef(): images(6) { }
    UnitViewDef(std::string name): name(name), hold_bpm(60), move_bpm(60), move_speed(60), images(6) { }
    UnitViewDef(std::string name, int hold_bpm, int move_bpm, int move_speed): name(name), hold_bpm(hold_bpm), move_bpm(move_bpm), move_speed(move_speed), images(6) { }
    ~UnitViewDef() { }

    std::string name;
    int hold_bpm;
    int move_bpm;
    int move_speed; // tiles per minute
    UnitImageMap images;
};

typedef std::map<std::string, UnitViewDef *> UnitViewDefMap;

#endif
