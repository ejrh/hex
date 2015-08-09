#include "common.h"

#include "hex/basics/error.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"
#include "hex/resources/resources.h"


void ResourceLoader::receive(boost::shared_ptr<Message> msg) {
    switch (msg->type) {
        case IncludeResource: {
            boost::shared_ptr<WrapperMessage<std::string> > upd = boost::dynamic_pointer_cast<WrapperMessage<std::string> >(msg);
            include(upd->data);
        } break;

        case IncludeIfResourceExists: {
            boost::shared_ptr<WrapperMessage<std::string> > upd = boost::dynamic_pointer_cast<WrapperMessage<std::string> >(msg);
            include(upd->data, true);
        } break;

        case ImageFile: {
            if (image_loader == NULL) {
                warn("Image loader is not defined");
                return;
            }
            boost::shared_ptr<WrapperMessage<std::string> > upd = boost::dynamic_pointer_cast<WrapperMessage<std::string> >(msg);
            load_image(upd->data);
        } break;

        case ImageSet: {
            boost::shared_ptr<WrapperMessage2<std::string, ImageSeries> > upd = boost::dynamic_pointer_cast<WrapperMessage2<std::string, ImageSeries> >(msg);
            resources->image_series[upd->data1] = upd->data2;
        } break;

        case CreateFactionView: {
            boost::shared_ptr<WrapperMessage<FactionViewDef> > upd = boost::dynamic_pointer_cast<WrapperMessage<FactionViewDef> >(msg);
            FactionViewDef *def = new FactionViewDef(upd->data);
            resources->faction_view_defs[def->name] = def;
        } break;

        case CreateTileView: {
            boost::shared_ptr<WrapperMessage<TileViewDef> > upd = boost::dynamic_pointer_cast<WrapperMessage<TileViewDef> >(msg);
            last_tile_view_def = resources->create_tile_view(upd->data);
        } break;

        case TileAnimation: {
            boost::shared_ptr<TileAnimationMessage> upd = boost::dynamic_pointer_cast<TileAnimationMessage>(msg);
            last_tile_view_def->animation.bpm = upd->data1;
            last_tile_view_def->animation.images = upd->data2;
        } break;

        case TileTransition: {
            boost::shared_ptr<TileTransitionMessage> upd = boost::dynamic_pointer_cast<TileTransitionMessage>(msg);
            last_tile_view_def->transitions.push_back(upd->data);
        } break;

        case TileRoads: {
            boost::shared_ptr<TileRoadsMessage> upd = boost::dynamic_pointer_cast<TileRoadsMessage>(msg);
            last_tile_view_def->roads = upd->data;
        } break;

        case TileFeature: {
            boost::shared_ptr<TileFeatureMessage> upd = boost::dynamic_pointer_cast<TileFeatureMessage>(msg);
            last_tile_view_def->features.push_back(upd->data);
        } break;

        case CreateUnitView: {
            boost::shared_ptr<WrapperMessage<UnitViewDef> > upd = boost::dynamic_pointer_cast<WrapperMessage<UnitViewDef> >(msg);
            UnitViewDef *def = new UnitViewDef(upd->data);
            resources->unit_view_defs[def->name] = def;
            last_unit_view_def = def;
        } break;

        case UnitHoldAnimation: {
            boost::shared_ptr<UnitHoldAnimationMessage> upd = boost::dynamic_pointer_cast<UnitHoldAnimationMessage>(msg);
            int facing = upd->data1;
            last_unit_view_def->hold_animations[facing].bpm = upd->data2;
            last_unit_view_def->hold_animations[facing].images = upd->data3;
        } break;

        case UnitMoveAnimation: {
            boost::shared_ptr<UnitMoveAnimationMessage> upd = boost::dynamic_pointer_cast<UnitMoveAnimationMessage>(msg);
            int facing = upd->data1;
            last_unit_view_def->move_animations[facing].bpm = upd->data2;
            last_unit_view_def->move_animations[facing].images = upd->data3;
        } break;

        case LoadSong: {
            boost::shared_ptr<LoadSongMessage> upd = boost::dynamic_pointer_cast<LoadSongMessage>(msg);
            load_song(upd->data);
        } break;

        default: {
            warn("Don't know how to read resources from message type: %s", get_message_type_name(msg->type).c_str());
        }
    }
}

void ResourceLoader::load(const std::string& filename) {
    trace("Loading resources from: %s", filename.c_str());
    current_files.push_back(filename);
    replay_messages(filename, *this);
    current_files.pop_back();
}

void ResourceLoader::include(const std::string& filename, bool skip_missing) {
    boost::filesystem::path path = boost::filesystem::path(current_files.back()).parent_path();
    path /= boost::filesystem::path(filename);
    std::string relative_filename = path.string();

    if (std::find(current_files.begin(), current_files.end(), relative_filename) != current_files.end()) {
        warn("Self-inclusion of resource file will be ignored: %s", relative_filename.c_str());
        return;
    }

    if (skip_missing && !boost::filesystem::exists(boost::filesystem::path(relative_filename))) {
        trace("Skipping missing resource: %s", relative_filename.c_str());
        return;
    }

    trace("Including resources from: %s", filename.c_str());
    current_files.push_back(relative_filename);
    replay_messages(relative_filename, *this);
    current_files.pop_back();
}

void ResourceLoader::load_image(const std::string& filename) {
    boost::filesystem::path path = boost::filesystem::path(current_files.back()).parent_path();
    path /= boost::filesystem::path(filename);
    std::string relative_filename = path.string();

    image_loader->load(relative_filename);
}

void ResourceLoader::load_song(const std::string& filename) {
    boost::filesystem::path path = boost::filesystem::path(current_files.back()).parent_path();
    path /= boost::filesystem::path(filename);
    std::string relative_filename = path.string();

    resources->songs.insert(relative_filename);
}
