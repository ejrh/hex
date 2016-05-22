#ifndef PROPERTIES_H
#define PROPERTIES_H

class Unit;

#define PROPERTY_TYPE(n) n,
enum PropertyType {
    UnknownPropertyType,
    #include "hex/game/property_types.h"
    NUM_PROPERTY_TYPES
};
#undef PROPERTY_TYPE

typedef std::map<PropertyType, int> Properties;

PropertyType get_property_type(const std::string& name);
const std::string get_property_type_name(const PropertyType property_type);

#endif
