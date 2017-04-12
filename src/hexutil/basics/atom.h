#ifndef ATOM_H
#define ATOM_H

#include "hexutil/basics/error.h"


class Atom {
public:
    Atom(): id(0) { }
    Atom(int id): id(id) { }

    bool operator==(const int& val) const {
        return id == val;
    }
    bool operator<(const Atom& other) const {
        return id < other.id;
    }

    operator int() const {
        return id;
    }

    // Defined below as it references AtomRegistry
    operator const std::string&() const;
    Atom(const char *name);
    Atom(const std::string& name);

private:
    int id;

    friend class AtomRegistry;
};

template<>
struct std::hash<Atom> {
    std::size_t operator()(const Atom& atom) const {
        return atom;
    }
};


class AtomRegistry {
public:
    static const Atom atom(const std::string& name) {
        auto found = str_to_id.find(name);
        if (found != str_to_id.end())
            return Atom(found->second);

        return register_atom(name, next_free_id);
    }

    static const std::string& name(const Atom& atom) {
        auto found = id_to_str.find(atom.id);
        if (found != id_to_str.end())
            return found->second;
        return empty_string;
    }

    static Atom register_atom(const std::string& name, int id);

private:
    static std::map<int, std::string> id_to_str;
    static std::map<std::string, int> str_to_id;
    static int next_free_id;
    static const std::string empty_string;
};

inline Atom::operator const std::string&() const {
    return AtomRegistry::name(*this);
}

inline Atom::Atom(const char *name): id(AtomRegistry::atom(name)) { }

inline Atom::Atom(const std::string& name): id(AtomRegistry::atom(name)) { }

inline std::ostream& operator<<(std::ostream& os, const Atom& atom) {
    return os << AtomRegistry::name(atom);
}

#endif
