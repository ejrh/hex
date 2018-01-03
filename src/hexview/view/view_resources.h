#ifndef VIEW_RESOURCES_H
#define VIEW_RESOURCES_H

#include "hexutil/basics/objmap.h"
#include "hexutil/messaging/loader.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"
#include "hexutil/scripting/scripting.h"

#include "hexav/graphics/graphics.h"
#include "hexav/resources/resources.h"

#include "hexview/view/view_def.h"


namespace hex {

class ViewResources: public Resources {
public:
    ViewResources();
    virtual ~ViewResources();
    void resolve_references();

    TileViewDef::pointer create_tile_view(const TileViewDef& def);
    FeatureViewDef::pointer create_feature_view(const FeatureViewDef& def);
    StructureViewDef::pointer create_structure_view(const StructureViewDef& data);
    TileViewDef::pointer get_tile_view_def(const std::string& name);
    FeatureViewDef::pointer get_feature_view_def(const std::string& name);
    UnitViewDef::pointer get_unit_view_def(const std::string& name);
    StructureViewDef::pointer get_structure_view_def(const std::string& name);
    FactionViewDef::pointer get_faction_view_def(const std::string& name);

public:
    StrMap<TileViewDef> tile_view_defs;
    StrMap<FeatureViewDef> feature_view_defs;
    StrMap<UnitViewDef> unit_view_defs;
    StrMap<StructureViewDef> structure_view_defs;
    StrMap<FactionViewDef> faction_view_defs;

    friend class ViewResourceLoader;
};

};

#endif
