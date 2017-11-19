#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/messaging/message.h"
#include "hexutil/messaging/receiver.h"
#include "hexutil/messaging/builtin_messages.h"
#include "hexutil/scripting/scripting.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_messages.h"

#include "hexview/resources/paint.h"
#include "hexview/resources/resources.h"
#include "hexview/resources/resource_loader.h"
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

        case ImageLibraries: {
            if (image_loader == NULL) {
                if (!warned_image_loader) {
                    BOOST_LOG_TRIVIAL(error) << "Image loader is not defined";
                    warned_image_loader = true;
                }
                return;
            }
            auto upd = dynamic_cast<ImageLibrariesMessage *>(msg);
            load_image_libraries(upd->data);
        } break;

        case ImageLibrary: {
            if (image_loader == NULL) {
                if (!warned_image_loader) {
                    BOOST_LOG_TRIVIAL(error) << "Image loader is not defined";
                    warned_image_loader = true;
                }
                return;
            }
            auto upd = dynamic_cast<ImageLibraryMessage *>(msg);
            load_image_library(upd->data1, upd->data2);
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
            last_feature_view_def = resources->create_feature_view(upd->data);
        } break;

        case FeaturePaint: {
            auto upd = dynamic_cast<FeaturePaintMessage *>(msg);
            last_feature_view_def->script = define_script("feature/" + last_feature_view_def->name, upd->data);
        } break;

        case CreateUnitView: {
            auto upd = dynamic_cast<CreateUnitViewMessage *>(msg);
            UnitViewDef::pointer def = boost::make_shared<UnitViewDef>(upd->data);
            resources->unit_view_defs.put_and_warn(def->name, def);
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
            last_structure_view_def = resources->create_structure_view(upd->data);
        } break;

        case StructurePaint: {
            auto upd = dynamic_cast<StructurePaintMessage *>(msg);
            last_structure_view_def->script = define_script("structure/" + last_structure_view_def->name, upd->data);
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

        case DefineScript: {
            auto upd = dynamic_cast<DefineScriptMessage *>(msg);
            if (!upd->data3) {
                define_script(upd->data1, upd->data2);
            } else {
                define_script(upd->data1, upd->data2, upd->data3);
            }
        } break;

        default: {
            BOOST_LOG_TRIVIAL(warning) << "Don't know how to read resources from message type: " << MessageTypeRegistry::get_message_type_name(msg->type);
        }
    }

    ++message_counter;
}

void ResourceLoader::load_image(const std::string& filename) {
    boost::filesystem::path path = boost::filesystem::path(get_current_file()).parent_path();
    path /= boost::filesystem::path(filename);
    std::string relative_filename = path.string();

    image_loader->load(relative_filename);
}

void ResourceLoader::load_image_libraries(const std::string& filename) {
    boost::filesystem::path path = boost::filesystem::path(get_current_file()).parent_path();
    path /= boost::filesystem::path(filename);
    if (!boost::filesystem::exists(path)) {
        BOOST_LOG_TRIVIAL(warning) << "Image libraries base does not exist: " << filename;
    }
    std::string relative_filename = path.string();

    image_loader->load_libraries(relative_filename);
}

void ResourceLoader::load_image_library(Atom name, const std::string& filename) {
    boost::filesystem::path path = boost::filesystem::path(get_current_file()).parent_path();
    path /= boost::filesystem::path(filename);
    if (!boost::filesystem::exists(path)) {
        BOOST_LOG_TRIVIAL(warning) << "Image library does not exist: " << filename;
    }
    std::string relative_filename = path.string();
    resources->image_libraries[name].reset(new ImageLibraryResource(relative_filename));

    // Set the image_loader on the Resources object, so it can lazily load the actual library when needed
    resources->image_loader = std::unique_ptr<ImageLoader>(new ImageLoader(*image_loader));
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

Script::pointer ResourceLoader::define_script(const std::string& name, Term *parameters, Term *instruction) {
    Script::pointer script = compile_script(name, parameters, instruction);

    BOOST_LOG_TRIVIAL(info) << "Added script: " << script->signature();
    resources->scripts.put_and_warn(name, script);
    return script;
}

std::string get_resource_basename(const std::string& filename) {
    size_t pos = filename.find_last_of("/");
    std::string basename(filename);
    if (pos != std::string::npos)
        basename = filename.substr(pos+1);
    std::transform(basename.begin(), basename.end(), basename.begin(), ::tolower);
    return basename;
}

bool has_extension(const std::string& filename, const std::string& ext) {
    return boost::algorithm::iends_with(filename, ext);
}
