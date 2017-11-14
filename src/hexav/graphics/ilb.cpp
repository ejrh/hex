#include "common.h"

#include "hexutil/basics/error.h"

#include "hexav/graphics/graphics.h"
#include "hexav/graphics/ilb.h"

#define MAX_NAME_LENGTH 100

long Reader::current_position() {
    return (long) file.tellg();
}

void Reader::set_position(long new_position) {
    file.seekg(new_position, std::ios_base::beg);
}

long Reader::size() {
    std::streamoff pos = file.tellg();
    file.seekg (0, file.end);
    std::streamoff size = file.tellg();
    file.seekg(pos, file.beg);
    return (long) size;
}

unsigned char Reader::read_byte() {
    char buffer[1];

    file.read(buffer, 1);

    return (unsigned char) buffer[0];
}

int Reader::read_short() {
    unsigned char buffer[2];

    file.read((char *) buffer, 2);

    unsigned int a = buffer[0];
    unsigned int b = buffer[1];

    return (short) (a | (b << 8));
}

int Reader::read_int() {
    unsigned char buffer[4];

    file.read((char *) buffer, 4);

    unsigned int a = buffer[0];
    unsigned int b = buffer[1];
    unsigned int c = buffer[2];
    unsigned int d = buffer[3];

    return (signed int) (a | (b << 8) | (c << 16) | (d << 24));
}

float Reader::read_float() {
    unsigned char buffer[4];

    file.read((char *) buffer, 4);

    return *(float *) buffer;
}

void Reader::read_data(char *target, int target_size) {
    file.read(target, target_size);
}

void Reader::read_RLE8(int expected_size, unsigned char transparent, unsigned char *target, int target_size) {
    while (expected_size > 0) {
        int scanline_len = read_int() - 4;
        expected_size -= 4;
        expected_size -= scanline_len;
        while (scanline_len > 0) {
            unsigned char value = read_byte();
            scanline_len--;

            unsigned char skip;
            if (value == transparent) {
                skip = read_byte();
                scanline_len--;
            } else {
                skip = 1;
            }

            while (skip > 0 && target_size > 0) {
                skip--;
                *target = value;
                target++;
                target_size--;
            }
        }
    }
}

void Reader::read_RLE16(int expected_size, unsigned short transparent, unsigned short *target, int target_size) {
    while (expected_size > 0) {
        int scanline_len = (read_int() - 4) / 2;
        bool skip_extra_one = (scanline_len % 2 != 0);
        if (scanline_len > 1000) {
            throw Error() << boost::format("Scanline length of %d is excessive") % scanline_len;
        }
        expected_size -= 4;
        expected_size -= 2 * scanline_len;
        while (scanline_len > 0) {
            unsigned short value = (unsigned short ) read_short();
            scanline_len--;

            unsigned short skip;
            if (value == transparent) {
                skip = (unsigned short ) (read_short() / 2);
                scanline_len--;
            } else {
                skip = 1;
            }

            while (skip > 0 && target_size > 0) {
                skip--;
                *target = value;
                target++;
                target_size -= 2;
            }
            if (skip != 0) {
                throw Error() << "RLE16 data exceeded allocated size";
            }
        }
        if (skip_extra_one) {
            read_short();
            expected_size -= 2;
        }
    }
}

void ILBReader::read_header(HeaderData &header) {
    header.magic = read_int();
    read_int(); // unknown value
    header.version = read_int();
    header.header_length = read_int();
    if (header.version == ILB_VERSION_4) {
        header.image_directory_length = read_int();
        header.file_length = read_int();
        header.num_palettes = read_int();
    } else {
        read_int(); // unknown (palette?)
        header.image_directory_length = 0;
        header.file_length = 0;
        header.num_palettes = 0;
    }
}

void ILBReader::read_palette(SDL_Color *palette_data) {
    read_int();  // id
    read_data((char *) palette_data, 1024);
}

void ILBReader::read_pixel_format(ImageData &image) {
    if (image.info_byte == 3) {
        int mode_int = read_int();
        image.show_mode = mode_int & 0xFF;
        if (image.show_mode == ILB_SHOW_BLENDED) {
            image.blend_mode = (mode_int >> 8) & 0xFF;
        } else {
            image.blend_mode = ILB_BLEND_USER;
        }
        image.blend_value = read_int();
    } else {
        image.blend_mode = ILB_BLEND_USER;
    }
    image.pixel_format = read_int();
}

void ILBReader::read_clip_info(ImageData &image) {
    image.clip_width = read_int();
    image.clip_height = read_int();
    image.clip_x_offset = read_int();
    image.clip_y_offset = read_int();
}

bool ILBReader::read_image(bool &composite, ImageData &image) {
    if (!composite) {
        image.id = read_int();
    }

    if (image.id == ILB_EOF_ID)
        return false;

    image.type = read_int();
    if (image.type == 256) {
        composite = true;
        image.type = read_int();
    }

    if (image.type == 0) {
        composite = false;
        read_int();
        return false;
    }

    image.info_byte = read_byte();

    int name_length = read_int();
    if (name_length < 0 || name_length > MAX_NAME_LENGTH)
        throw Error() << "Abnormal image name length: " <<  name_length;

    char buffer[MAX_NAME_LENGTH + 1];
    read_data(buffer, name_length);
    buffer[name_length] = '\0';
    image.name.assign(buffer);

    image.width = read_int();
    image.height = read_int();
    image.x_offset = read_int();
    image.y_offset = read_int();

    image.sub_id = read_int(); // composites only

    read_byte();  // unknown byte

    image.data_size = read_int();

    if (image.info_byte != 1) {
        image.data_offset = header.image_directory_length + read_int();
    } else {
        image.data_offset = 0;
    }

    image.offset_width = read_int();
    image.offset_height = read_int();

    switch (image.type) {
        case ILB_TYPE_RLESPRITE08:
            read_byte();  // 3 unknowns
            if (image.info_byte == 3) {
                read_int();
                read_int();
            }
            image.palette_num = read_int();
            if (image.palette_num < 0 || image.palette_num >= header.num_palettes)
                BOOST_LOG_TRIVIAL(warning) << "Palette number out of range: " << image.palette_num;
            read_clip_info(image);
            image.transparency_index = read_int();
            read_int(); // unknown
            image.show_mode = 0;
            image.blend_mode = 0;
            image.blend_value = 0;
            image.pixel_format = 0;
            break;

        case ILB_TYPE_PICTURE16:
            read_pixel_format(image);
            image.clip_x_offset = 0;
            image.clip_y_offset = 0;
            image.clip_width = image.width;
            image.clip_height = image.height;
            break;

        case ILB_TYPE_RLESPRITE16:
        case ILB_TYPE_TRANSPARENT_RLESPRITE16:
            read_pixel_format(image);
            read_clip_info(image);
            image.transparency_colour = read_int();
            read_int(); // unknown
            break;

        case ILB_TYPE_SPRITE16:
            read_pixel_format(image);
            read_clip_info(image);
            image.transparency_colour = read_int();
            break;

        default:
            throw Error() << "Unknown image type: " << image.type;
    }

    if (image.info_byte == 1) {
        image.data_offset = current_position();
    }

    if (!composite) {
        int end_int = read_int();
        if (end_int != ILB_EOF_ID)
            throw Error() << "Missing image end symbol";
    }

    return true;
}

char *ILBReader::read_pixel_data(ImageData &image) {
    int data_pos = image.data_offset;
    int pixel_data_size = image.get_pixel_size();
    char *pixel_data = new char[pixel_data_size];

    long saved_position = current_position();
    set_position(data_pos);

    if (image.type == ILB_TYPE_PICTURE16) {
        read_data(pixel_data, pixel_data_size);
    } else if (image.type == ILB_TYPE_RLESPRITE16 || image.type == ILB_TYPE_TRANSPARENT_RLESPRITE16) {
        read_RLE16(image.data_size, (unsigned short) image.transparency_colour, (unsigned short *) pixel_data, pixel_data_size);
    } else if (image.type == ILB_TYPE_RLESPRITE08) {
        read_RLE8(image.data_size, (unsigned char) image.transparency_index, (unsigned char *) pixel_data, pixel_data_size);
    } else if (image.type == ILB_TYPE_SPRITE16) {
        read_data(pixel_data, pixel_data_size);
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Don't know how to read data for ILB image type " << image.type;
    }

    set_position(saved_position);

    return pixel_data;
}

static void fix_transparency(char *pixel_data, ImageData &image, SDL_Surface *surface) {
    for (int i = 0; i < image.clip_height * image.clip_width; i++) {
        if (((Uint16 *) pixel_data)[i] == image.transparency_colour) {
            Uint32 old_key;
            SDL_GetColorKey(surface, &old_key);
            Uint32 key = ((Uint32 *) surface->pixels)[i];
            key &= ~0xFF000000;
            if (old_key != key) {
                SDL_SetColorKey(surface, SDL_TRUE, key);
                //BOOST_LOG_TRIVIAL(debug) << boost::format("Changed colour key from %08x to %08x") % old_key % key;
            }
            return;
        }
    }
    //BOOST_LOG_TRIVIAL(debug) << "Couldn't find any transparent pixels";
}

Image *ILBReader::create_image(char *pixel_data, ImageData &image) {
    SDL_Surface *surface;

    if (image.type == ILB_TYPE_PICTURE16 && image.pixel_format == ILB_FORMAT_565) {
        surface = SDL_CreateRGBSurfaceFrom(pixel_data, image.width, image.height, 16, image.width*2, 0x0000f800,0x000007e0,0x0000001f,0);
    } else if ((image.type == ILB_TYPE_RLESPRITE16 || image.type == ILB_TYPE_TRANSPARENT_RLESPRITE16) && image.pixel_format == ILB_FORMAT_565) {
        surface = SDL_CreateRGBSurfaceFrom(pixel_data, image.clip_width, image.clip_height, 16, image.clip_width*2, 0x0000f800,0x000007e0,0x0000001f,0);
        SDL_SetColorKey(surface, SDL_TRUE, image.transparency_colour);
    } else if (image.type == ILB_TYPE_RLESPRITE08) {
        surface = SDL_CreateRGBSurfaceFrom(pixel_data, image.clip_width, image.clip_height, 8, image.clip_width, 0,0,0,0);
        SDL_Palette *palette = palettes[image.palette_num];
        if (palette != NULL)
            SDL_SetSurfacePalette(surface, palette);
        SDL_SetColorKey(surface, SDL_TRUE, image.transparency_index);
    } else if (image.type == ILB_TYPE_SPRITE16 && image.pixel_format == ILB_FORMAT_565) {
        surface = SDL_CreateRGBSurfaceFrom(pixel_data, image.clip_width, image.clip_height, 16, image.clip_width*2, 0x0000f800,0x000007e0,0x0000001f,0);
        SDL_SetColorKey(surface, SDL_TRUE, image.transparency_colour);
    } else {
        BOOST_LOG_TRIVIAL(warning) << "Don't know how to create surface for ILB image type " << image.type << " with pixel format " << image.pixel_format;
        return NULL;
    }

    if (surface == NULL) {
        BOOST_LOG_TRIVIAL(warning) << "SDL Error while creating image: " << SDL_GetError();
        return NULL;
    }

    Uint32 pf = SDL_GetWindowPixelFormat(graphics->window);
    SDL_Surface *new_surface = SDL_ConvertSurfaceFormat(surface, pf, 0);
    SDL_FreeSurface(surface);

    // This is a silly step that we need to do because SDL uses different rounding when converting
    // the colour key from 565 format to RGBA, vs. converting the surface's pixels.
    if (image.pixel_format == ILB_FORMAT_565) {
        fix_transparency(pixel_data, image, new_surface);
    }

    /*
    std::ostringstream oss;
    oss << boost::format("%d_%d_%s.png") % image.id % image.sub_id % image.name;
    if (IMG_SavePNG(new_surface, oss.str().c_str()) != 0) {
        throw Error() << "SDL Image error:" << SDL_GetError();
    }
    BOOST_LOG_TRIVIAL(debug) << "Saved " << oss;
    */

    SDL_Texture *texture = SDL_CreateTextureFromSurface(graphics->renderer, new_surface);
    SDL_FreeSurface(new_surface);

    if (!texture) {
        BOOST_LOG_TRIVIAL(warning) << "SDL Error while creating texture: " << SDL_GetError();
        return NULL;
    }

    if (image.blend_mode == ILB_BLEND_INTENSITY) {
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_ADD);
        int bv = image.blend_value * 255 / 100;
        SDL_SetTextureAlphaMod(texture, bv);
    }

    Image *im = new Image(image.id, texture);
    im->clip_x_offset = image.clip_x_offset;
    im->clip_y_offset = image.clip_y_offset;
    im->width = image.width;
    im->height = image.height;

    ++graphics->ilb_image_load_counter;

    return im;
}

void ILBReader::read(ImageMap& image_set, const std::string& prefix) {
    read_header(header);
    if (header.magic != ILB_MAGIC)
        throw Error() << boost::format("ILB magic number missing, was: %08x") % header.magic;

    if (header.version != ILB_VERSION_3 && header.version != ILB_VERSION_4)
        throw Error() << boost::format("ILB version not recognised: %0.1f") % header.version;

    for (int i = 0; i < header.num_palettes; i++) {
        SDL_Color palette_data[256];
        read_palette(palette_data);

        SDL_Palette *palette = SDL_AllocPalette(256);
        SDL_SetPaletteColors(palette, palette_data, 0, 256);
        palettes.push_back(palette);
    }

    bool finished = false;
    bool composite = false;

    while (!finished) {
        ImageData image;
        if (!read_image(composite, image)) {
            if (image.id == ILB_EOF_ID) {
                finished = true;
                break;
            } else if (image.type == 0)
                continue;
        }

        if (image.data_size > 0) {
            char *pixel_data = read_pixel_data(image);
            Image *im = create_image(pixel_data, image);
            delete[] pixel_data;

            if (im != NULL) {
                std::ostringstream ss;
                ss << prefix << image.name << "/" << image.id << "-" << image.sub_id;
                std::string name = ss.str();
                if (image_set.find(name) != image_set.end()) {
                    BOOST_LOG_TRIVIAL(warning) << "Overwriting previously loaded image: " << name;
                }
                //BOOST_LOG_TRIVIAL(trace) << "Loaded image: " << name;
                image_set[name] = im;
            }
        }
    }
}

void load_ilb(const std::string& filename, Graphics *graphics, ImageMap& image_set) {
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (file.fail())
        throw Error() << "Could not open file: " << filename;

    ILBReader reader(file, graphics);
    reader.read(image_set);
}
