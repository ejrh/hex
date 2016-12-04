#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"

#include "hexview/resources/resources.h"
#include "hexview/resources/resource_messages.h"


void ResourceLoader::handle_message(Message *msg) {
    switch (msg->type) {
        case ImageFile: {
            if (image_loader == NULL) {
                if (!warned_image_loader) {
                    BOOST_LOG_TRIVIAL(error) << "Image loader is not defined";
                    warned_image_loader = true;
                }
                return;
            }
            auto upd = dynamic_cast<ImageFileMessage *>(msg);
            load_image(upd->data);
        } break;

        case ImageSet: {
            auto upd = dynamic_cast<ImageSetMessage *>(msg);
            resources->image_series[upd->data1] = upd->data2;
        } break;

        case CreateFactionView: {
            auto upd = dynamic_cast<CreateFactionViewMessage *>(msg);
            FactionViewDef::pointer def = boost::make_shared<FactionViewDef>(upd->data);
            resources->faction_view_defs.put_and_warn(def->name, def);
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
            last_tile_view_def = resources->create_tile_view(upd->data);
        } break;

        case TileAnimation: {
            auto upd = dynamic_cast<TileAnimationMessage *>(msg);
            last_tile_view_def->animation.bpm = upd->data1;
            last_tile_view_def->animation.images = upd->data2;
        } break;

        case TileTransition: {
            auto upd = dynamic_cast<TileTransitionMessage *>(msg);
            last_tile_view_def->transitions.push_back(upd->data);
        } break;

        case TileRoads: {
            auto upd = dynamic_cast<TileRoadsMessage *>(msg);
            last_tile_view_def->roads = upd->data;
        } break;

        case TileFeature: {
            auto upd = dynamic_cast<TileFeatureMessage *>(msg);
            last_tile_view_def->features.push_back(upd->data);
        } break;

        case CreateUnitView: {
            auto upd = dynamic_cast<CreateUnitViewMessage *>(msg);
            UnitViewDef::pointer def = boost::make_shared<UnitViewDef>(upd->data);
            resources->unit_view_defs.put_and_warn(def->name, def);
            last_unit_view_def = def;
        } break;

        case UnitHoldAnimation: {
            auto upd = dynamic_cast<UnitHoldAnimationMessage *>(msg);
            int facing = upd->data1;
            last_unit_view_def->hold_animations[facing].bpm = upd->data2;
            last_unit_view_def->hold_animations[facing].images = upd->data3;
        } break;

        case UnitMoveAnimation: {
            auto upd = dynamic_cast<UnitMoveAnimationMessage *>(msg);
            int facing = upd->data1;
            last_unit_view_def->move_animations[facing].bpm = upd->data2;
            last_unit_view_def->move_animations[facing].images = upd->data3;
        } break;

        case UnitAttackAnimation: {
            auto upd = dynamic_cast<UnitAttackAnimationMessage *>(msg);
            int facing = upd->data1;
            last_unit_view_def->attack_animations[facing].bpm = upd->data2;
            last_unit_view_def->attack_animations[facing].images = upd->data3;
        } break;

        case UnitRecoilAnimation: {
            auto upd = dynamic_cast<UnitRecoilAnimationMessage *>(msg);
            int facing = upd->data1;
            last_unit_view_def->recoil_animations[facing].bpm = upd->data2;
            last_unit_view_def->recoil_animations[facing].images = upd->data3;
        } break;

        case UnitDieAnimation: {
            auto upd = dynamic_cast<UnitDieAnimationMessage *>(msg);
            int facing = upd->data1;
            last_unit_view_def->die_animations[facing].bpm = upd->data2;
            last_unit_view_def->die_animations[facing].images = upd->data3;
        } break;

        case UnitShadowAnimation: {
            auto upd = dynamic_cast<UnitShadowAnimationMessage *>(msg);
            int facing = upd->data1;
            last_unit_view_def->shadow_animations[facing].bpm = upd->data2;
            last_unit_view_def->shadow_animations[facing].images = upd->data3;
        } break;

        case UnitSounds: {
            auto upd = dynamic_cast<UnitSoundsMessage *>(msg);
            int posture = upd->data1;;
            last_unit_view_def->sounds[posture].sounds = upd->data2;
        } break;

        case CreateStructureView: {
            auto upd = dynamic_cast<CreateStructureViewMessage *>(msg);
            last_structure_view_def = resources->create_structure_view(upd->data);
        } break;

        case StructureAnimation: {
            auto upd = dynamic_cast<StructureAnimationMessage *>(msg);
            last_structure_view_def->centre_x = upd->data1;
            last_structure_view_def->centre_y = upd->data2;
            last_structure_view_def->animation.bpm = upd->data3;
            last_structure_view_def->animation.images = upd->data4;
        } break;

        case LoadSong: {
            auto upd = dynamic_cast<LoadSongMessage *>(msg);
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
            auto upd = dynamic_cast<SoundFileMessage *>(msg);
            load_sound(upd->data);
        } break;

        default: {
            BOOST_LOG_TRIVIAL(warning) << "Don't know how to read resources from message type: " << MessageTypeRegistry::get_message_type_name(msg->type);
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
