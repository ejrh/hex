#ifndef DATUM_H
#define DATUM_H

#include "hexutil/basics/atom.h"
#include <boost/concept_check.hpp>


namespace hex {

struct Datum;

template<typename T>
const T get_datum_as(const Datum& datum);

// The variant must be wrapped like this, otherwise the compiler will get confused about which overload to use
struct Datum {
    Datum(): value(0) { }
    Datum(int x): value(x) { }
    Datum(const std::string& x): value(x) { }

    boost::variant<Atom, int, float, std::string> value;

    Datum& operator=(const Datum& x) {
        value = x.value;
        return *this;
    }

    bool operator==(const Datum& x) const { return value == x.value; }

    Datum& operator=(const Atom& x) { value = x; return *this; }
    bool operator==(const Atom& x) const { return boost::get<Atom>(value) == x; }

    Datum& operator=(const int& x) { value = x; return *this; }
    bool operator==(const int& x) const { return boost::get<int>(value) == x; }

    template<typename T>
    operator T() const { return get_as<T>(); }

    template<typename T>
    bool is() const {
        return boost::get<T>(&value) != nullptr;
    }

    /* Use this when the datum is *known* to be of type T. */
    template<typename T>
    const T get_as_known() const {
        return boost::get<T>(value);
    }

    template<typename T>
    const T get_as() const {
        return get_datum_as<T>(*this);
    }

    std::string get_as_str() const;
    Atom get_as_atom() const;
    int get_as_int() const;
};


template<>
inline const int get_datum_as<int>(const Datum& datum) {
    return datum.get_as_int();
}

template<>
inline const Atom get_datum_as<Atom>(const Datum& datum) {
    return datum.get_as_atom();
}

template<>
inline const std::string get_datum_as<std::string>(const Datum& datum) {
    return datum.get_as_str();
}


std::ostream& operator<<(std::ostream& os, const Datum& atom);

};

#endif
