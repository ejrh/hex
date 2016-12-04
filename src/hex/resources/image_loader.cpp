#include "common.h"

#include "hexav/graphics/ilb.h"

#include "hex/resources/resources.h"


extern void load_hss(const std::string& filename, Graphics *graphics, ImageMap& image_set);

extern void load_image(const std::string& filename, Graphics *graphics, ImageMap& image_set);

void ImageLoader::load(const std::string& filename) {
    if (filename.rfind(".png") == filename.size() - 4) {
        load_image(filename, graphics, resources->images);
    } else if (filename.rfind(".ILB") == filename.size() - 4) {
        BOOST_LOG_TRIVIAL(warning) << "Loading images from: " << filename;
        load_ilb(filename, graphics, resources->images);
    } else if (filename.rfind(".hss") == filename.size() - 4) {
        BOOST_LOG_TRIVIAL(warning) << "Loading images from: " << filename;
        load_hss(filename, graphics, resources->images);
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Don't know how to load images from: " << filename;
    }
}
