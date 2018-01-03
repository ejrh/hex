#include "common.h"

#include "hexutil/basics/error.h"

#include "hexav/graphics/graphics.h"


namespace hex {

void save_image(const std::string& filename, SDL_Surface *surface) {
    if (!IMG_SavePNG(surface,filename.c_str())) {
        throw throw Error() << "SDL Image error while saving to '" << filename << "': " << SDL_GetError();
    }
}

//TODO - a function that can save textures, for making screenshots, etc.

};
