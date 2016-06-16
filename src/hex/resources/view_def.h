#ifndef VIEW_DATA_H
#define VIEW_DATA_H

#include "hex/resources/image_ref.h"


class AnimationDef {
public:
    AnimationDef(): bpm(60) { }

public:
    int bpm;
    ImageSeries images;
};

class FactionViewDef: public boost::enable_shared_from_this<FactionViewDef> {
public:
    typedef boost::shared_ptr<FactionViewDef> pointer;

    FactionViewDef() { }
    FactionViewDef(std::string name): name(name) { }
    FactionViewDef(std::string name, int r, int g, int b): name(name), r(r), g(g), b(b) { }
    std::string name;
    int r, g, b;
};

class TransitionDef {
public:
    TransitionDef() { }

    std::set<int> dirs;
    std::set<std::string> type_names;
    ImageSeries images;
};

class FeatureDef {
public:
    FeatureDef() { }

    int centre_x, centre_y;
    ImageSeries images;
};

class TileViewDef: public boost::enable_shared_from_this<TileViewDef> {
public:
    typedef boost::shared_ptr<TileViewDef> pointer;

    TileViewDef() { }
    TileViewDef(std::string name): name(name) { }
    ~TileViewDef() { }

    std::string name;
    int r, g, b;
    AnimationDef animation;
    std::vector<TransitionDef> transitions;
    ImageSeries roads;
    std::vector<FeatureDef> features;
};

class UnitViewDef: public boost::enable_shared_from_this<UnitViewDef> {
public:
    typedef boost::shared_ptr<UnitViewDef> pointer;

    UnitViewDef(): hold_animations(6), move_animations(6), attack_animations(6), recoil_animations(6), die_animations(6), move_speed(60) { }
    UnitViewDef(std::string name): name(name), hold_animations(6), move_animations(6), move_speed(60) { }
    ~UnitViewDef() { }

    std::string name;
    std::vector<AnimationDef> hold_animations;
    std::vector<AnimationDef> move_animations;
    std::vector<AnimationDef> attack_animations;
    std::vector<AnimationDef> recoil_animations;
    std::vector<AnimationDef> die_animations;
    int move_speed; // tiles per minute
};

class StructureViewDef: public boost::enable_shared_from_this<StructureViewDef> {
public:
    typedef boost::shared_ptr<StructureViewDef> pointer;

    StructureViewDef() { }
    StructureViewDef(const std::string& name): name(name), centre_x(0), centre_y(0) { }

    std::string name;
    int centre_x, centre_y;
    AnimationDef animation;
};

#endif
