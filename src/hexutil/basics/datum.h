#ifndef DATUM_H
#define DATUM_H

#include "hexutil/basics/atom.h"


// The variant must be wrapped like this, otherwise the compiler will get confused about which overload to use
struct Datum {
    Datum(): value(1) { }
    boost::variant<Atom, int, float, std::string> value;

    Datum& operator=(const int& x) { value = x; return *this; }
    bool operator==(const int& x) const { return boost::get<int>(value) == x; }

    template<typename T>
    operator T() const { return boost::get<T>(value); }

    template<typename T>
    bool is() const {
        return boost::get<T>(&value) != nullptr;
    }

    template<typename T>
    const T& get() const {
        return boost::get<T>(value);
    }

    std::string get_as_str() const;
    Atom get_as_atom() const;
};

std::ostream& operator<<(std::ostream& os, const Datum& atom);


#endif
