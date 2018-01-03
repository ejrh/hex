#ifndef ILB_H
#define ILB_H

#include "hexav/graphics/graphics.h"


namespace hex {

#define ILB_MAGIC 0x424C4904
#define ILB_VERSION_3 0x40400000
#define ILB_VERSION_4 0x40800000
#define ILB_EOF_ID (int) 0xFFFFFFFF

#define ILB_TYPE_RLESPRITE08 2
#define ILB_TYPE_PICTURE16 16
#define ILB_TYPE_RLESPRITE16 17
#define ILB_TYPE_TRANSPARENT_RLESPRITE16 18
#define ILB_TYPE_SPRITE16 22

#define ILB_SHOW_OPAQUE 0
#define ILB_SHOW_TRANSPARENT 1
#define ILB_SHOW_BLENDED 2

#define ILB_BLEND_USER 0
#define ILB_BLEND_ALPHA 1
#define ILB_BLEND_BRIGHTEN 2
#define ILB_BLEND_INTENSITY 3
#define ILB_BLEND_SHADOW 4
#define ILB_BLEND_LINEAR_ALPHA 5

#define ILB_FORMAT_565 0x56509310

class Reader {
private:
    std::istream& file;
public:
    Reader(std::istream& file): file(file) { }
    virtual ~Reader() { }

    long current_position();
    void set_position(long new_position);
    long size();
    unsigned char read_byte();
    int read_short();
    int read_int();
    float read_float();
    void read_data(char *target, int target_size);
    void read_RLE8(int expected_size, unsigned char transparent, unsigned char *target, int target_size);
    void read_RLE16(int expected_size, unsigned short transparent, unsigned short *target, int target_size);
};

struct HeaderData {
    int magic;
    int version;
    int header_length;
    int image_directory_length;
    int file_length;
    int num_palettes;
};

struct ImageData {
    int id;
    int type;
    unsigned char info_byte;
    std::string name;
    int width, height;
    int x_offset, y_offset;
    int sub_id;
    int data_size;
    int data_offset;
    int offset_width, offset_height;
    int palette_num;
    int clip_width, clip_height;
    int clip_x_offset, clip_y_offset;
    int transparency_index;
    int transparency_colour;
    unsigned char show_mode, blend_mode;
    int blend_value;
    int pixel_format;

    int get_pixel_size() {
        return clip_height * clip_width * (type == ILB_TYPE_RLESPRITE08 ? 1 : 2);
    }
};

class ILBReader: protected Reader {
public:
    ILBReader(std::istream &file, Graphics *graphics): Reader(file), graphics(graphics) { };
    virtual ~ILBReader() {
        for (auto i = palettes.begin(); i != palettes.end(); i++) {
            SDL_FreePalette(*i);
        }
    }
    void read(ImageMap& image_set, const std::string& prefix = "");

private:
    void read_header(HeaderData &header);
    void read_palette(SDL_Color *palette_data);
    void read_pixel_format(ImageData &image);
    void read_clip_info(ImageData &image);
    char *read_pixel_data(ImageData &image);
    bool read_image(bool &composite, ImageData &image);
    Image *create_image(char *pixel_data, ImageData &image);

    HeaderData header;
    std::vector<SDL_Palette *> palettes;
    Graphics *graphics;
};

void load_ilb(const std::string& filename, Graphics *graphics, ImageMap& image_set);

};

#endif
