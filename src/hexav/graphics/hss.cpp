#include "common.h"

#include "hexutil/basics/error.h"

#include "hexav/graphics/graphics.h"

#include "ilb.h"


namespace hex {

typedef std::pair<int, int> Range;

class TOC {
public:
    TOC(Reader& reader, int end_pos) {
        read_from(reader);
        this->end_pos = end_pos;
    }
    TOC(Reader& reader, const Range& pos) {
        reader.set_position(pos.first);
        end_pos = pos.second;
        read_from(reader);
    }

    std::pair<int, int> lookup_position(int key) const;
    void print_to(std::ostream& os) const;

private:
    void read_from(Reader& reader);

public:
    int end_pos;
    int toc_start, toc_end;
    std::vector<std::pair<int, Range> > entries;
};

void TOC::read_from(Reader& reader) {
    toc_start = reader.current_position();

    int num_entries = reader.read_byte();
    int short_entries, long_entries;
    if (num_entries & 0x80) {
        short_entries = num_entries & ~0x80;
        long_entries = reader.read_int();
    } else {
        short_entries = num_entries;
        long_entries = 0;
    }

    int last_key = -1, last_offset;

    for (int i = 0; i < short_entries; i++) {
        int key = reader.read_byte();
        int offset = reader.read_byte();

        if (last_key != -1) {
            entries.push_back(std::pair<int, Range>(last_key, Range(last_offset, offset)));
        }
        last_key = key;
        last_offset = offset;
    }

    for (int i = 0; i < long_entries; i++) {
        int key = reader.read_int();
        int offset = reader.read_int();

        if (last_key != -1) {
            entries.push_back(std::pair<int, Range>(last_key, Range(last_offset, offset)));
        }
        last_key = key;
        last_offset = offset;
    }

    toc_end = reader.current_position();

    if (last_key != -1) {
        entries.push_back(std::pair<int, Range>(last_key, Range(last_offset, end_pos - toc_end)));
    }

    for (auto iter = entries.begin(); iter != entries.end(); iter++) {
        iter->second.first += toc_end;
        iter->second.second += toc_end;
    }
}

Range TOC::lookup_position(int key) const {
    for (auto iter = entries.begin(); iter != entries.end(); iter++) {
        if (iter->first == key)
            return iter->second;
    }

    return Range(0, 0);
}

void TOC::print_to(std::ostream& os) const {
    os << boost::format("TOC from %x to %x [\n") % toc_start % toc_end;
    for (auto iter = entries.begin(); iter != entries.end(); iter++) {
        os << boost::format("    %x -> %x to %x\n") % iter->first % iter->second.first % iter->second.second;
    }
    os << "]\n";
}


class HSSReader: protected Reader {
public:
    HSSReader(std::istream &file, Graphics *graphics): Reader(file), graphics(graphics) { };
    void read(ImageMap& image_set);

private:
    Graphics *graphics;
};

void HSSReader::read(ImageMap& image_set) {
    // Structure of an HSS file:
    // First 4 bytes are the offset of the top-level TOC.
    // We look up the sequence of sub-TOCs 0, 5, 1.  This final TOC contains the entries.

    Range toc_pos(read_int(), size());
    TOC toc(*this, toc_pos);
    //toc.print_to(std::cerr);

    Range toc_0_pos = toc.lookup_position(0x00);
    TOC toc_0(*this, toc_0_pos);
    //toc_0.print_to(std::cerr);

    Range toc_0_5_pos = toc_0.lookup_position(0x05);
    TOC toc_0_5(*this, toc_0_5_pos);
    //toc_0_5.print_to(std::cerr);

    Range toc_0_5_1_pos = toc_0_5.lookup_position(0x01);
    TOC toc_0_5_1(*this, toc_0_5_1_pos);
    //toc_0_5_1.print_to(std::cerr);

    //BOOST_LOG_TRIVIAL(trace) << boost::format("Processing %d HSS entries") % toc_0_5_1.entries.size();

    int sequence_number = 0;

    for (auto iter = toc_0_5_1.entries.begin(); iter != toc_0_5_1.entries.end(); iter++) {
        // For each entry, the first 4 bytes are flags of some kind, then the entry's TOC appears
        // Key 0x01 is the unique id, if it exists (id is 0 otherwise)
        // Key 0x08 is the ILB file, if it exists
        Range entry_toc_pos = toc_0_5_1.lookup_position(iter->first);
        entry_toc_pos.first += 4;
        TOC entry_toc(*this, entry_toc_pos);
        //entry_toc.print_to(std::cerr);
        Range id_pos = entry_toc.lookup_position(0x01);
        if (id_pos.first != 0) {
            set_position(id_pos.first);
            //TODO - store id somewhere?
            read_int();
        }

        Range ilb_pos = entry_toc.lookup_position(0x08);
        if (ilb_pos.first == 0)
            continue;

        // TODO don't read into char arrays then into strings then into stringstreams just to load from this position
        int ilb_len = ilb_pos.second - ilb_pos.first;
        char *buffer = new char[ilb_len + 1];
        set_position(ilb_pos.first);
        read_data(buffer, ilb_len);
        buffer[ilb_len] = 0;
        std::string buf_str(buffer, ilb_len);
        delete[] buffer;
        std::istringstream s(buf_str);
        ILBReader ilb_reader(s, graphics);
        ImageMap ilb_images;
        ilb_reader.read(ilb_images);

        //BOOST_LOG_TRIVIAL(trace) << boost::format("Processing entry %d") % id;

        // then copy each image in the ILB into the result map, with an id taken from the entry id + the image sub_id
        for (auto iter = ilb_images.begin(); iter != ilb_images.end(); iter++) {
            std::ostringstream ss;
            ss << boost::format("%04d/%s") % (sequence_number++) % iter->first;
            image_set[ss.str()] = iter->second;
            //BOOST_LOG_TRIVIAL(trace) << boost::format("Loaded image %s from ILB") % ss.str();
        }
    }
}

void load_hss(const std::string& filename, Graphics *graphics, ImageMap& image_set) {
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (file.fail())
        throw Error() << "Could not open file: " << filename;

    HSSReader reader(file, graphics);
    reader.read(image_set);
}

};
