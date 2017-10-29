#ifndef PROPERTIES_H
#define PROPERTIES_H

#include "hexutil/basics/atom.h"
#include "hexutil/basics/datum.h"


class Properties {
public:
    int count(const Atom& name) const {
        return data.count(name);
    }

    std::unordered_map<Atom, Datum>::const_iterator find(const Atom& name) const {
        return data.find(name);
    }

    bool contains(const Atom& name) const {
        return count(name) > 0;
    }

    bool contains(const Atom& name, const Properties& parent) const {
        return contains(name) || parent.contains(name);
    }

    const Datum& get(const Atom& name) const {
        auto iter = data.find(name);
        if (iter != data.end()) {
            return iter->second;
        }
        static Datum default_value;
        return default_value;
    }

    const Datum& get(const Atom& name, const Properties& parent) const {
        if (contains(name))
            return get(name);
        return parent.get(name);
    }

    template<typename T>
    void set(const Atom& name, const T& value) {
        data[name].value = value;
    }

    template<typename T>
    const Datum& increment(const Atom& name, const T& delta) {
        const T& current = get(name);
        set<T>(name, current + delta);
        return get(name);
    }

    Datum& operator[](const Atom& name) { return data[name]; }

public:
    std::unordered_map<Atom, Datum> data;
};

std::ostream& operator<<(std::ostream& os, const Properties& p);


#endif
