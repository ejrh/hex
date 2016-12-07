#include "common.h"

#include "hexutil/basics/datum.h"


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
