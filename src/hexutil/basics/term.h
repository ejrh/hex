#ifndef TERM_H
#define TERM_H

class Term {
public:
    virtual ~Term() { }

    virtual void print_to_stream(std::ostream& os) const = 0;

public:
    int line_no;
};


class DatumTerm: public Term {
public:
    DatumTerm(const Datum& datum): datum(datum) { }

    virtual void print_to_stream(std::ostream& os) const;

public:
    Datum datum;
};


class CompoundTerm: public Term {
public:
    CompoundTerm(const Atom& functor): functor(functor) { }
    CompoundTerm(std::vector<Term *>& terms): functor(list_functor) {
        for (auto iter = terms.begin(); iter != terms.end(); iter++)
            add_subterm(*iter);
    }

    void add_subterm(Term *subterm);

    bool is_list() const {
        return functor == list_functor;
    }

    virtual void print_to_stream(std::ostream& os) const;

public:
    Atom functor;
    std::vector<std::unique_ptr<Term> > subterms;

    static Atom list_functor;
};


std::ostream& operator<<(std::ostream& os, const Term *term);
std::ostream& operator<<(std::ostream& os, const Term& term);

#endif
