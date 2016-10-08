#include "common.h"

#include "hex/basics/error.h"
#include "hex/game/game.h"
#include "hex/game/game_messages.h"
#include "hex/messaging/message.h"
#include "hex/messaging/receiver.h"
#include "hex/resources/resources.h"


void ResourceLoader::handle_message(boost::shared_ptr<Message> msg) {
    switch (msg->type) {
        case ImageFile: {
            if (image_loader == NULL) {
                if (!warned_image_loader) {
                    BOOST_LOG_TRIVIAL(error) << "Image loader is not defined";
                    warned_image_loader = true;
                }
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
            FactionViewDef::pointer def = boost::make_shared<FactionViewDef>(upd->data);
            resources->faction_view_defs.put_and_warn(def->name, def);
            last_faction_view_def = def;
        } break;

        case FactionImageSet: {
            boost::shared_ptr<FactionImageSetMessage> upd = boost::dynamic_pointer_cast<FactionImageSetMessage>(msg);
            if (upd->data1 == "BIG_FLAGS")
                last_faction_view_def->big_flag_images = upd->data2;
            else if (upd->data1 == "SMALL_FLAGS")
                last_faction_view_def->small_flag_images = upd->data2;
            else if (upd->data1 == "SHIELDS")
                last_faction_view_def->shield_images = upd->data2;
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
            UnitViewDef::pointer def = boost::make_shared<UnitViewDef>(upd->data);
            resources->unit_view_defs.put_and_warn(def->name, def);
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

        case UnitAttackAnimation: {
            boost::shared_ptr<UnitAttackAnimationMessage> upd = boost::dynamic_pointer_cast<UnitAttackAnimationMessage>(msg);
            int facing = upd->data1;
            last_unit_view_def->attack_animations[facing].bpm = upd->data2;
            last_unit_view_def->attack_animations[facing].images = upd->data3;
        } break;

        case UnitRecoilAnimation: {
            boost::shared_ptr<UnitRecoilAnimationMessage> upd = boost::dynamic_pointer_cast<UnitRecoilAnimationMessage>(msg);
            int facing = upd->data1;
            last_unit_view_def->recoil_animations[facing].bpm = upd->data2;
            last_unit_view_def->recoil_animations[facing].images = upd->data3;
        } break;

        case UnitDieAnimation: {
            boost::shared_ptr<UnitDieAnimationMessage> upd = boost::dynamic_pointer_cast<UnitDieAnimationMessage>(msg);
            int facing = upd->data1;
            last_unit_view_def->die_animations[facing].bpm = upd->data2;
            last_unit_view_def->die_animations[facing].images = upd->data3;
        } break;

        case UnitShadowAnimation: {
            boost::shared_ptr<UnitShadowAnimationMessage> upd = boost::dynamic_pointer_cast<UnitShadowAnimationMessage>(msg);
            int facing = upd->data1;
            last_unit_view_def->shadow_animations[facing].bpm = upd->data2;
            last_unit_view_def->shadow_animations[facing].images = upd->data3;
        } break;

        case UnitSounds: {
            boost::shared_ptr<UnitSoundsMessage> upd = boost::dynamic_pointer_cast<UnitSoundsMessage>(msg);
            int posture = upd->data1;;
            last_unit_view_def->sounds[posture].sounds = upd->data2;
        } break;

        case CreateStructureView: {
            boost::shared_ptr<CreateStructureViewMessage> upd = boost::dynamic_pointer_cast<CreateStructureViewMessage>(msg);
            last_structure_view_def = resources->create_structure_view(upd->data);
        } break;

        case StructureAnimation: {
            boost::shared_ptr<StructureAnimationMessage> upd = boost::dynamic_pointer_cast<StructureAnimationMessage>(msg);
            last_structure_view_def->centre_x = upd->data1;
            last_structure_view_def->centre_y = upd->data2;
            last_structure_view_def->animation.bpm = upd->data3;
            last_structure_view_def->animation.images = upd->data4;
        } break;

        case LoadSong: {
            boost::shared_ptr<LoadSongMessage> upd = boost::dynamic_pointer_cast<LoadSongMessage>(msg);
            load_song(upd->data);
        } break;

        case SoundFile: {
            if (sound_loader == NULL) {
                if (!warned_sound_loader) {
                    BOOST_LOG_TRIVIAL(error) << "Sound loader is not defined";
                    warned_sound_loader = true;
                }
                return;
            }
            boost::shared_ptr<SoundFileMessage> upd = boost::dynamic_pointer_cast<SoundFileMessage>(msg);
            load_sound(upd->data);
        } break;

        default: {
            BOOST_LOG_TRIVIAL(warning) << "Don't know how to read resources from message type: " << get_message_type_name(msg->type);
        }
    }
}

void ResourceLoader::load_image(const std::string& filename) {
    boost::filesystem::path path = boost::filesystem::path(get_current_file()).parent_path();
    path /= boost::filesystem::path(filename);
    std::string relative_filename = path.string();

    image_loader->load(relative_filename);
}

void ResourceLoader::load_song(const std::string& filename) {
    boost::filesystem::path path = boost::filesystem::path(get_current_file()).parent_path();
    path /= boost::filesystem::path(filename);
    std::string relative_filename = path.string();

    resources->songs.insert(relative_filename);
}

void ResourceLoader::load_sound(const std::string& filename) {
    boost::filesystem::path path = boost::filesystem::path(get_current_file()).parent_path();
    path /= boost::filesystem::path(filename);
    std::string relative_filename = path.string();

    sound_loader->load(relative_filename);
}
