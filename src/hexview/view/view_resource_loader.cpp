#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"
#include "hexutil/messaging/builtin_messages.h"
#include "hexutil/scripting/scripting.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"

#include "hexav/resources/paint.h"
#include "hexav/resources/resources.h"
#include "hexav/resources/resource_loader.h"

#include "hexview/view/view_resource_loader.h"
#include "hexview/view/view_resource_messages.h"


namespace hex {

void ViewResourceLoader::handle_message(Message *msg) {
    switch (msg->type) {
        case CreateFactionView: {
            auto upd = dynamic_cast<CreateFactionViewMessage *>(msg);
            FactionViewDef::pointer def = boost::make_shared<FactionViewDef>(upd->data);
            view_resources->faction_view_defs.put_and_warn(def->name, def);
            last_faction_view_def = def;
        } break;

        case FactionImageSet: {
            auto upd = dynamic_cast<FactionImageSetMessage *>(msg);
            if (upd->data1 == "BIG_FLAGS")
                last_faction_view_def->big_flag_images = upd->data2;
            else if (upd->data1 == "SMALL_FLAGS")
                last_faction_view_def->small_flag_images = upd->data2;
            else if (upd->data1 == "SHIELDS")
                last_faction_view_def->shield_images = upd->data2;
        } break;

        case CreateTileView: {
            auto upd = dynamic_cast<CreateTileViewMessage *>(msg);
            last_tile_view_def = view_resources->create_tile_view(upd->data);
        } break;

        case TilePaint: {
            auto upd = dynamic_cast<TilePaintMessage *>(msg);
            last_tile_view_def->script = define_script("tile/" + last_tile_view_def->name, upd->data);
        } break;

        case TransitionPaint: {
            auto upd = dynamic_cast<TransitionPaintMessage *>(msg);
            last_tile_view_def->transition_script = define_script("transition/" + last_tile_view_def->name, upd->data);
        } break;

        case CreateFeatureView: {
            auto upd = dynamic_cast<CreateFeatureViewMessage *>(msg);
            last_feature_view_def = view_resources->create_feature_view(upd->data);
        } break;

        case FeaturePaint: {
            auto upd = dynamic_cast<FeaturePaintMessage *>(msg);
            last_feature_view_def->script = define_script("feature/" + last_feature_view_def->name, upd->data);
        } break;

        case CreateUnitView: {
            auto upd = dynamic_cast<CreateUnitViewMessage *>(msg);
            UnitViewDef::pointer def = boost::make_shared<UnitViewDef>(upd->data);
            view_resources->unit_view_defs.put_and_warn(def->name, def);
            last_unit_view_def = def;
        } break;

        case UnitSounds: {
            auto upd = dynamic_cast<UnitSoundsMessage *>(msg);
            int posture = upd->data1;;
            last_unit_view_def->sounds[posture].sounds = upd->data2;
        } break;

        case UnitPaint: {
            auto upd = dynamic_cast<UnitPaintMessage *>(msg);
            last_unit_view_def->script = define_script("unit/" + last_unit_view_def->name, upd->data);
        } break;

        case CreateStructureView: {
            auto upd = dynamic_cast<CreateStructureViewMessage *>(msg);
            last_structure_view_def = view_resources->create_structure_view(upd->data);
        } break;

        case StructurePaint: {
            auto upd = dynamic_cast<StructurePaintMessage *>(msg);
            last_structure_view_def->script = define_script("structure/" + last_structure_view_def->name, upd->data);
        } break;

        default: {
            ResourceLoader::handle_message(msg);
        }
    }

    ++view_message_counter;
}

};
