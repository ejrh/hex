#include "common.h"

#include "hex/game/properties.h"


class PropertyMap {
public:

#define PROPERTY_TYPE(n) data[#n] = n;
    PropertyMap() {
        #include "hex/game/property_types.h"
    }
#undef PROPERTY_TYPE

    std::map<std::string, PropertyType> data;
};

PropertyMap property_map;

PropertyType get_property_type(const std::string& name) {
    std::map<std::string, PropertyType>::iterator iter = property_map.data.find(name);
    if (iter != property_map.data.end())
        return iter->second;
    BOOST_LOG_TRIVIAL(warning) << boost::format("Unknown property type: %s") % name;
    return UnknownPropertyType;
}

const std::string get_property_type_name(const PropertyType property_type) {

#define PROPERTY_TYPE(n) case n: return std::string(#n); break;
    switch (property_type) {
        #include "hex/game/property_types.h"
        default:
            return std::string("unknown");
    }
#undef PROPERTY_TYPE
}
