#ifndef DATUM_H
#define DATUM_H


// The variant must be wrapped like this, otherwise the compiler will get confused about which overload to use
struct Datum {
    Datum(): value(1) { }
    boost::variant<int, float, std::string> value;

    Datum& operator=(const int& x) { value = x; return *this; }
    bool operator==(const int& x) const { return boost::get<int>(value) == x; }
};

std::ostream& operator<<(std::ostream& os, const Datum& atom);


#endif
