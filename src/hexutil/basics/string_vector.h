#ifndef STRING_VECTOR_H
#define STRING_VECTOR_H

namespace hex {

class CompressableStringVector {
public:
    std::vector<std::string> data;

    size_t size() const {
        return data.size();
    }

    void push_back(const std::string& str) {
        return data.push_back(str);
    }

    void clear() {
        data.clear();
    }
};

extern void compress_string_vector(const std::vector<std::string>& input, std::vector<std::string>& output);
extern void decompress_string_vector(const std::vector<std::string>& input, std::vector<std::string>& output);

inline void compress_string_vector(const CompressableStringVector& input, std::vector<std::string>& output) {
    compress_string_vector(input.data, output);
}

inline void decompress_string_vector(const std::vector<std::string>& input, CompressableStringVector& output) {
    decompress_string_vector(input, output.data);
}

};

#endif
