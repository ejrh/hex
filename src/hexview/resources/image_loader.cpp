#include "common.h"

#include "hexav/graphics/ilb.h"

#include "hexview/resources/resources.h"
#include "hexview/resources/resource_loader.h"


extern void load_hss(const std::string& filename, Graphics *graphics, ImageMap& image_set);

extern void load_image(const std::string& filename, Graphics *graphics, ImageMap& image_set);

void ImageLoader::load(const std::string& filename) {
    if (has_extension(filename, ".png")) {
        load_image(filename, graphics, resources->images);
    } else if (has_extension(filename, ".ilb")) {
        BOOST_LOG_TRIVIAL(warning) << "Loading images from: " << filename;
        load_ilb(filename, graphics, resources->images);
    } else if (has_extension(filename, ".hss")) {
        BOOST_LOG_TRIVIAL(warning) << "Loading images from: " << filename;
        load_hss(filename, graphics, resources->images);
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Don't know how to load images from: " << filename;
    }
}

void ImageLoader::load_libraries(const std::string& filename) {
    if (has_extension(filename, ".hss")) {
        BOOST_LOG_TRIVIAL(warning) << "Loading image libraries from: " << filename;
        ImageMap hss_images;
        load_hss(filename, graphics, hss_images);

        std::unordered_map<Atom, std::unique_ptr<ImageLibraryResource> > loaded_libraries;

        for (auto iter = hss_images.begin(); iter != hss_images.end(); iter++) {
            std::vector<std::string> parts;
            boost::split(parts, iter->first, boost::is_any_of("/."));
            std::string& name = parts[1];
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            if (loaded_libraries.find(name) == loaded_libraries.end())
                loaded_libraries[name].reset(new ImageLibraryResource(name));

            ImageLibraryResource *lib = loaded_libraries[name].get();
            lib->images[lib->images.size()].reset(iter->second);
            lib->loaded = true;
        }

        for (auto iter = loaded_libraries.begin(); iter != loaded_libraries.end(); iter++) {
            BOOST_LOG_TRIVIAL(info) << boost::format("Loaded image library '%s' with %d entries") % iter->first % iter->second->images.size();
            resources->image_libraries[iter->first] = std::move(iter->second);
        }
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Don't know how to load image libraries from: " << filename;
    }

}

void ImageLoader::load_library(Atom name, const std::string& filename) {
    ImageMap images;
    if (has_extension(filename, ".ilb")) {
        BOOST_LOG_TRIVIAL(info) << "Loading image library: " << filename;
        load_ilb(filename, graphics, images);
    } else if (has_extension(filename, ".hss")) {
        BOOST_LOG_TRIVIAL(info) << "Loading image library: " << filename;
        load_hss(filename, graphics, images);
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Don't know how to load image library: " << filename;
    }

    ImageLibraryResource *lib = resources->image_libraries[name].get();
    for (auto iter = images.begin(); iter != images.end(); iter++) {
        Image *image = iter->second;
        lib->images[image->id].reset(image);
    }
    lib->loaded = true;
}
