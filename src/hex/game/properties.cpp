#include "common.h"

#include "hex/game/properties.h"


class PropertyMap {
public:

#define PROPERTY_NAME(n) data[#n] = n;
    PropertyMap() {
        #include "hex/game/property_names.h"
    }
#undef PROPERTY_NAME

    std::map<std::string, PropertyName> data;
};

PropertyMap property_map;

PropertyName get_property_name(const std::string& name) {
    auto iter = property_map.data.find(name);
    if (iter != property_map.data.end())
        return iter->second;
    BOOST_LOG_TRIVIAL(warning) << boost::format("Unknown property name: %s") % name;
    return UnknownPropertyName;
}

const std::string get_property_name_str(const PropertyName property_name) {

#define PROPERTY_NAME(n) case n: return std::string(#n); break;
    switch (property_name) {
        #include "hex/game/property_names.h"
        default:
            return std::string("unknown");
    }
#undef PROPERTY_NAME
}

std::ostream& operator<<(std::ostream& os, const Properties& p) {
    os << "{";
    bool first = true;
    for (auto iter = p.data.begin(); iter != p.data.end(); iter++) {
        if (!first)
            os << ", ";
        else
            first = false;

        os << get_property_name_str(iter->first) << ": " << iter->second.value;
    }
    os << "}";
    return os;
}
