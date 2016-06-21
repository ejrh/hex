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

class Properties {
public:
    int count(const PropertyType& type) const {
        return data.count(type);
    }

    std::map<PropertyType, int>::const_iterator find(const PropertyType& type) const {
        return data.find(type);
    }

    int& operator[](const PropertyType& type) { return data[type]; }

public:
    std::map<PropertyType, int> data;
};

PropertyType get_property_type(const std::string& name);
const std::string get_property_type_name(const PropertyType property_type);

std::ostream& operator<<(std::ostream& os, const Properties& p);

#endif
