#include "common.h"

#include "hexutil/basics/properties.h"


std::ostream& operator<<(std::ostream& os, const Properties& p) {
    os << "{";
    bool first = true;
    for (auto iter = p.data.begin(); iter != p.data.end(); iter++) {
        if (!first)
            os << ", ";
        else
            first = false;

        os << iter->first << ": " << iter->second.value;
    }
    os << "}";
    return os;
}
