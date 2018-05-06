#ifndef ATOM_H
#define ATOM_H

#include "hexutil/basics/error.h"


namespace hex {

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

    bool operator!=(const int& val) const {
        return id != val;
    }

    bool operator!=(const Atom& other) const {
        return id != other.id;
    }

    bool operator<(const Atom& other) const {
        return id < other.id;
    }

    operator int() const {
        return id;
    }

    operator bool() const {
        return id;
    }

    // Defined below as they reference AtomRegistry
    operator const std::string&() const;
    Atom(int id, const char *name);
    Atom(const char *name);
    Atom(const std::string& name);

    const std::string& name() const;
    const char *c_str() const;

private:
    int id;

    friend class AtomRegistry;
};

class AtomRegistry {
public:
    static AtomRegistry& get_instance() {
        static AtomRegistry instance;
        return instance;
    }

    const Atom atom(const std::string& name);
    const std::string& name(const Atom atom);
    int id(const char *name);
    int id(const std::string& name);
    int id(const Atom atom);
	void log_registered_atoms();
    Atom register_atom(const std::string& name, int id);

public:
    Atom empty;

private:
    AtomRegistry();

    Atom register_atom_nolock(const std::string& name, int id);
    int get_free_id();

private:
    boost::mutex mtx;
    std::unordered_map<int, std::string> id_to_str;
    std::unordered_map<std::string, int> str_to_id;
    int next_free_id;
    const std::string empty_string;
};

inline Atom::operator const std::string&() const {
    return AtomRegistry::get_instance().name(*this);
}

inline Atom::Atom(int id, const char *name): id(id) {
    AtomRegistry::get_instance().register_atom(name, id);
}

inline Atom::Atom(const char *name): id(AtomRegistry::get_instance().id(name)) { }

inline Atom::Atom(const std::string& name): id(AtomRegistry::get_instance().id(name)) { }

inline std::ostream& operator<<(std::ostream& os, const Atom& atom) {
    return os << AtomRegistry::get_instance().name(atom);
}

inline Atom operator+(const Atom& atom1, const Atom& atom2) {
    return AtomRegistry::get_instance().atom(AtomRegistry::get_instance().name(atom1) + AtomRegistry::get_instance().name(atom2));
}

inline const std::string& Atom::name() const {
    return AtomRegistry::get_instance().name(*this);
}

inline const char *Atom::c_str() const {
    return AtomRegistry::get_instance().name(*this).c_str();
}

inline Atom operator+(const char *a, Atom& other) {
    return Atom(a + AtomRegistry::get_instance().name(other));
}

};

namespace std {
    template<>
    struct hash<hex::Atom> {
        size_t operator()(const hex::Atom& atom) const {
            return hex::AtomRegistry::get_instance().id(atom);
        }
    };
}

#endif
