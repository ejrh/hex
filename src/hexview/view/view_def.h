#ifndef VIEW_DATA_H
#define VIEW_DATA_H

#include "hexav/audio/audio.h"

#include "hexav/resources/image_ref.h"
#include "hexav/resources/sound_ref.h"
#include "hexav/resources/paint.h"


namespace hex {

class SoundDef {
public:
    SoundDef() { }

public:
    SoundSeries sounds;
};

class FactionViewDef: public boost::enable_shared_from_this<FactionViewDef> {
public:
    typedef boost::shared_ptr<FactionViewDef> pointer;

    FactionViewDef() { }
    FactionViewDef(Atom name): name(name) { }
    FactionViewDef(Atom name, int r, int g, int b): name(name), r(r), g(g), b(b) { }
    Atom name;
    int r, g, b;
    ImageSeries big_flag_images;
    ImageSeries small_flag_images;
    ImageSeries shield_images;
};

class TileViewDef: public boost::enable_shared_from_this<TileViewDef> {
public:
    typedef boost::shared_ptr<TileViewDef> pointer;

    TileViewDef() { }
    TileViewDef(Atom name): name(name) { }
    ~TileViewDef() { }

    Atom name;
    int r, g, b;
    Script::pointer script;
    Script::pointer transition_script;
};

class FeatureViewDef: public boost::enable_shared_from_this<FeatureViewDef> {
public:
    typedef boost::shared_ptr<FeatureViewDef> pointer;

    FeatureViewDef() { }
    FeatureViewDef(Atom name): name(name) { }
    ~FeatureViewDef() { }

    Atom name;
    Script::pointer script;
};

#define NUM_POSTURES 6

class UnitViewDef: public boost::enable_shared_from_this<UnitViewDef> {
public:
    typedef boost::shared_ptr<UnitViewDef> pointer;

    UnitViewDef() { }
    UnitViewDef(Atom name):
        name(name) { }
    ~UnitViewDef() { }

    Atom name;
    Script::pointer script;
    std::array<SoundDef, NUM_POSTURES> sounds;
};

class StructureViewDef: public boost::enable_shared_from_this<StructureViewDef> {
public:
    typedef boost::shared_ptr<StructureViewDef> pointer;

    StructureViewDef() { }
    StructureViewDef(const Atom name): name(name), script(nullptr) { }

    Atom name;
    Script::pointer script;
};

};

#endif
