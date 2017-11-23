#ifndef VIEW_DATA_H
#define VIEW_DATA_H

#include "hexav/audio/audio.h"

#include "hexav/resources/image_ref.h"
#include "hexav/resources/sound_ref.h"
#include "hexav/resources/paint.h"


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
    FactionViewDef(std::string name): name(name) { }
    FactionViewDef(std::string name, int r, int g, int b): name(name), r(r), g(g), b(b) { }
    std::string name;
    int r, g, b;
    ImageSeries big_flag_images;
    ImageSeries small_flag_images;
    ImageSeries shield_images;
};

class TileViewDef: public boost::enable_shared_from_this<TileViewDef> {
public:
    typedef boost::shared_ptr<TileViewDef> pointer;

    TileViewDef() { }
    TileViewDef(std::string name): name(name) { }
    ~TileViewDef() { }

    std::string name;
    int r, g, b;
    Script::pointer script;
    Script::pointer transition_script;
};

class FeatureViewDef: public boost::enable_shared_from_this<FeatureViewDef> {
public:
    typedef boost::shared_ptr<FeatureViewDef> pointer;

    FeatureViewDef() { }
    FeatureViewDef(std::string name): name(name) { }
    ~FeatureViewDef() { }

    std::string name;
    Script::pointer script;
};

#define NUM_POSTURES 6

class UnitViewDef: public boost::enable_shared_from_this<UnitViewDef> {
public:
    typedef boost::shared_ptr<UnitViewDef> pointer;

    UnitViewDef() { }
    UnitViewDef(std::string name):
        name(name) { }
    ~UnitViewDef() { }

    std::string name;
    Script::pointer script;
    std::array<SoundDef, NUM_POSTURES> sounds;
};

class StructureViewDef: public boost::enable_shared_from_this<StructureViewDef> {
public:
    typedef boost::shared_ptr<StructureViewDef> pointer;

    StructureViewDef() { }
    StructureViewDef(const std::string& name): name(name), script(nullptr) { }

    std::string name;
    Script::pointer script;
};

#endif
