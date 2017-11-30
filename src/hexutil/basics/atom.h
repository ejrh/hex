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

    bool operator==(const Atom& other) const {
        return id == other.id;
    }

    bool operator<(const Atom& other) const {
        return id < other.id;
    }

    /*operator int() const {
        return id;
    }*/

    // Defined below as they reference AtomRegistry
    operator const std::string&() const;
    Atom(int id, const char *name);
    Atom(const char *name);
    Atom(const std::string& name);

private:
    int id;

    friend class AtomRegistry;
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

    static inline int id(const char *name) {
        return atom(name).id;
    }

    static inline int id(const std::string& name) {
        return atom(name).id;
    }

    static inline int id(const Atom& atom) {
        return atom.id;
    }

    static Atom register_atom(const std::string& name, int id);

public:
    static Atom empty;

private:
    static std::unordered_map<int, std::string> id_to_str;
    static std::unordered_map<std::string, int> str_to_id;
    static int next_free_id;
    static const std::string empty_string;
};

inline Atom::operator const std::string&() const {
    return AtomRegistry::name(*this);
}

inline Atom::Atom(int id, const char *name): id(id) {
    AtomRegistry::register_atom(name, id);
}

inline Atom::Atom(const char *name): id(AtomRegistry::id(name)) { }

inline Atom::Atom(const std::string& name): id(AtomRegistry::id(name)) { }

inline std::ostream& operator<<(std::ostream& os, const Atom& atom) {
    return os << AtomRegistry::name(atom);
}

namespace std {
    template<>
    struct hash<Atom> {
        size_t operator()(const Atom& atom) const {
            return AtomRegistry::id(atom);
        }
    };
}

#endif
