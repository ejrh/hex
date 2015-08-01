#ifndef VIEW_DATA_H
#define VIEW_DATA_H

#include "hex/graphics/graphics.h"
#include "hex/resources/image_ref.h"

class AnimationDef {
public:
    AnimationDef(): bpm(60) { }

public:
    int bpm;
    ImageSeries images;
};

class FactionViewDef {
public:
    FactionViewDef() { }
    FactionViewDef(std::string name, int r, int g, int b): name(name), r(r), g(g), b(b) { }

    std::string name;
    int r, g, b;
};

typedef std::map<std::string, FactionViewDef *> FactionViewDefMap;

class TileViewDef {
public:
    TileViewDef() { }
    TileViewDef(std::string name): name(name) { }
    ~TileViewDef() { }

    std::string name;
    AnimationDef animation;
};

typedef std::map<std::string, TileViewDef *> TileViewDefMap;


class UnitViewDef {
public:
    UnitViewDef(): move_speed(60), hold_animations(6), move_animations(6) { }
    UnitViewDef(std::string name): move_speed(60), hold_animations(6), move_animations(6) { }
    ~UnitViewDef() { }

    std::string name;
    std::vector<AnimationDef> hold_animations;
    std::vector<AnimationDef> move_animations;
    int move_speed; // tiles per minute
};

typedef std::map<std::string, UnitViewDef *> UnitViewDefMap;

#endif
