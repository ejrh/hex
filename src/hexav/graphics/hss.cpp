#include "common.h"

#include "hexutil/basics/error.h"

#include "hexav/graphics/graphics.h"

#include "ilb.h"

class HSSReader: protected Reader {
public:
    HSSReader(std::istream &file, Graphics *graphics): Reader(file), graphics(graphics) { };
    void read(ImageMap& image_set);

private:
    Graphics *graphics;
};

void HSSReader::read(ImageMap& image_set) {
    unsigned char *buffer;
    long file_size = size();
    buffer = new unsigned char[file_size];

    read_data((char *) buffer, file_size);

    unsigned char magic_0 = ILB_MAGIC & 0xFF;
    unsigned char magic_1 = (ILB_MAGIC >> 8) & 0xFF;
    unsigned char magic_2 = (ILB_MAGIC >> 16) & 0xFF;
    unsigned char magic_3 = ILB_MAGIC >> 24;

    long pos = 0;
    while (pos < file_size - 3) {
        if (buffer[pos] == magic_0 && buffer[pos+1] == magic_1 && buffer[pos+2] == magic_2 && buffer[pos+3] == magic_3) {
            std::string buf_str((char *) (buffer + pos), file_size - pos);
            std::istringstream s(buf_str);
            ILBReader ilb_reader(s, graphics);
            ilb_reader.read(image_set);
        }
        pos++;
    }

    delete[] buffer;
}

void load_hss(const std::string& filename, Graphics *graphics, ImageMap& image_set) {
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (file.fail())
        throw Error() << "Could not open file: " << filename;

    HSSReader reader(file, graphics);
    reader.read(image_set);
}