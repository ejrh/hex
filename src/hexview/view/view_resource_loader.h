#ifndef VIEW_RESOURCE_LOADER_H
#define VIEW_RESOURCE_LOADER_H

#include "hexutil/basics/statistics.h"
#include "hexutil/messaging/loader.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"

#include "hexav/resources/resources.h"
#include "hexav/resources/resource_loader.h"

#include "hexview/view/view_resources.h"


class ViewResourceLoader: public ResourceLoader {
public:
    ViewResourceLoader(ViewResources *resources, ImageLoader *image_loader, SoundLoader *sound_loader):
            ResourceLoader(resources, image_loader, sound_loader),
            view_resources(resources),
            last_faction_view_def(), last_tile_view_def(), last_unit_view_def(), last_structure_view_def(),
            view_message_counter("view.resource.message") { }

    void handle_message(Message *msg);

private:
    ViewResources *view_resources;

    FactionViewDef::pointer last_faction_view_def;
    TileViewDef::pointer last_tile_view_def;
    FeatureViewDef::pointer last_feature_view_def;
    UnitViewDef::pointer last_unit_view_def;
    StructureViewDef::pointer last_structure_view_def;

    Counter view_message_counter;
};


#endif
