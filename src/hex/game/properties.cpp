#include "common.h"

#include "hex/game/properties.h"


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

void register_property_names() {
#define PROPERTY_NAME(n) AtomRegistry::register_atom(#n, n);
#include "hex/game/property_names.h"
#undef PROPERTY_NAME
}
