#include "common.h"

#include "hexutil/basics/string_vector.h"


namespace hex {

void compress_string_vector(const std::vector<std::string>& input, std::vector<std::string>& output) {
    std::unordered_map<std::string, std::string> dictionary;

    for (unsigned int i = 0; i < input.size(); i++) {
        const std::string& input_str = input[i];
        auto found = dictionary.find(input_str);
        if (found != dictionary.end()) {
            const std::string& mapped_str = found->second;
            output.push_back(mapped_str);
        } else {
            std::ostringstream ss;
            unsigned int next_id = dictionary.size() + 1;
            ss << next_id;
            const std::string mapped_str = ss.str();
            dictionary[input_str] = mapped_str;
            output.push_back(input_str);
        }
    }
}

void decompress_string_vector(const std::vector<std::string>& input, std::vector<std::string>& output) {
    std::unordered_map<std::string, std::string> dictionary;

    for (unsigned int i = 0; i < input.size(); i++) {
        const std::string& input_str = input[i];
        auto found = dictionary.find(input_str);
        if (found != dictionary.end()) {
            const std::string& mapped_str = found->second;
            output.push_back(mapped_str);
        } else {
            std::ostringstream ss;
            unsigned int next_id = dictionary.size() + 1;
            ss << next_id;
            const std::string mapped_str = ss.str();
            dictionary[mapped_str] = input_str;
            output.push_back(input_str);
        }
    }
}

};
