#include "common.h"

#include "hex/graphics/ilb.h"
#include "hex/resources/resources.h"


extern void load_hss(const std::string& filename, Graphics *graphics, ImageMap& image_set);

extern void load_image(const std::string& filename, Graphics *graphics, ImageMap& image_set);

void ImageLoader::load(const std::string& filename) {
    if (filename.rfind(".png") == filename.size() - 4) {
        load_image(filename, graphics, resources->images);
    } else if (filename.rfind(".ILB") == filename.size() - 4) {
        trace("Loading images from: %s", filename.c_str());
        load_ilb(filename, graphics, resources->images);
    } else if (filename.rfind(".hss") == filename.size() - 4) {
        trace("Loading images from: %s", filename.c_str());
        load_hss(filename, graphics, resources->images);
    } else {
        warn("Don't know how to load images from: %s", filename.c_str());
    }
}
