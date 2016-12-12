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
