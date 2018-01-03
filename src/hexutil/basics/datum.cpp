#include "common.h"

#include "hexutil/basics/datum.h"


namespace hex {

class ostream_visitor: public boost::static_visitor<> {
public:
    ostream_visitor(std::ostream& os): os(os) { }

    void operator()(const Atom& atom) const {
        os << atom;
    }

    void operator()(const int& i) const {
        os << i;
    }

    void operator()(const std::string& str) const {
        os << str;
    }

private:
    std::ostream& os;
};

std::ostream& operator<<(std::ostream& os, const Datum& datum) {
    boost::apply_visitor(ostream_visitor(os), datum.value);
    return os;
}

std::string Datum::get_as_str() const {
    std::ostringstream ss;
    ss << *this;
    return ss.str();
}

Atom Datum::get_as_atom() const {
    std::ostringstream ss;
    ss << *this;
    return Atom(ss.str());
}

int Datum::get_as_int() const {
    if (is<int>()) {
        return get<int>();
    } else if (is<float>()) {
        return get<float>();
    } else {
        return 0;
    }
}

};
