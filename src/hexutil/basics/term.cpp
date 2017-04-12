#include "common.h"

#include "hexutil/basics/atom.h"
#include "hexutil/basics/datum.h"
#include "hexutil/basics/term.h"

void DatumTerm::print_to_stream(std::ostream& os) const {
    os << datum;
}

void CompoundTerm::add_subterm(Term *subterm) {
    subterms.push_back(std::unique_ptr<Term>(subterm));
}

void CompoundTerm::print_to_stream(std::ostream& os) const {
    if (is_list()) {
        os << "[";
    } else {
        os << functor << "(";
    }
    bool done_first = false;
    for (auto iter = subterms.begin(); iter != subterms.end(); iter++) {
        if (done_first) {
            os << ",";
        } else {
            done_first = true;
        }

        os << iter->get();
    }
    if (is_list()) {
        os << "]";
    } else {
        os << ")";
    }
}

std::ostream& operator<<(std::ostream& os, const Term *term) {
    term->print_to_stream(os);
    return os;
}


std::ostream& operator<<(std::ostream& os, const Term& term) {
    term.print_to_stream(os);
    return os;
}


Atom CompoundTerm::list_functor("[]");
