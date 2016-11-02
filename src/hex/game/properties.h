#ifndef PROPERTIES_H
#define PROPERTIES_H

#define PROPERTY_NAME(n) n,
enum PropertyName {
    UnknownPropertyName,
    #include "hex/game/property_names.h"
    NUM_PROPERTY_NAMES
};
#undef PROPERTY_NAME

// The variant must be wrapped like this, otherwise the compiler will get confused about which overload to use
struct PropertyValue {
    PropertyValue(): value(1) { }
    boost::variant<int, float, std::string> value;
    PropertyValue& operator=(const int& x) { value = x; return *this; }
    bool operator==(const int& x) const { return boost::get<int>(value) == x; }
};

class Properties {
public:
    int count(const PropertyName& name) const {
        return data.count(name);
    }

    std::map<PropertyName, PropertyValue>::const_iterator find(const PropertyName& name) const {
        return data.find(name);
    }

    bool contains(const PropertyName& name) const {
        return count(name) > 0;
    }

    bool contains(const PropertyName& name, const Properties& parent) const {
        return contains(name) || parent.contains(name);
    }

    template<typename T>
    const T& get(const PropertyName& name) const {
        auto iter = data.find(name);
        if (iter != data.end()) {
            return boost::get<const T>(iter->second.value);
        }
        static T default_value;
        return default_value;
    }

    template<typename T>
    const T& get(const PropertyName& name, const Properties& parent) const {
        if (contains(name))
            return get<T>(name);
        return parent.get<T>(name);
    }

    template<typename T>
    void set(const PropertyName& name, const T& value) {
        data[name].value = value;
    }

    template<typename T>
    const T& increment(const PropertyName& name, const T& delta) {
        const T& current = get<T>(name);
        set<T>(name, current + delta);
        return get<T>(name);
    }

    PropertyValue& operator[](const PropertyName& name) { return data[name]; }

public:
    std::map<PropertyName, PropertyValue> data;
};

PropertyName get_property_name(const std::string& name);
const std::string get_property_name_str(const PropertyName name);

std::ostream& operator<<(std::ostream& os, const Properties& p);

#endif
