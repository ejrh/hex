#include "common.h"

#include "hexutil/basics/atom.h"


namespace hex {

const int FIRST_FREE_ID = 1000001;

AtomRegistry::AtomRegistry():
    empty(0),
    next_free_id(FIRST_FREE_ID) {

    /* Explicitly associate 0 with "", as the AtomRegistry is not available for use until this constructor ends! */
    register_atom("", 0);
}


const Atom AtomRegistry::atom(const std::string& name) {
    boost::lock_guard<boost::mutex> guard(mtx);

    auto found = str_to_id.find(name);
    if (found != str_to_id.end())
        return Atom(found->second);

    return register_atom_nolock(name, get_free_id());
}

const std::string& AtomRegistry::name(const Atom atom) {
    boost::lock_guard<boost::mutex> guard(mtx);

    auto found = id_to_str.find(atom.id);
    if (found != id_to_str.end())
        return found->second;
    
    return empty_string;
}

int AtomRegistry::id(const char *name) {
    return atom(name).id;
}

int AtomRegistry::id(const std::string& name) {
    return atom(name).id;
}

int AtomRegistry::id(const Atom atom) {
    return atom.id;
}

void AtomRegistry::log_registered_atoms() {
    boost::lock_guard<boost::mutex> guard(mtx);

    std::ostringstream ss;
    ss << "REGISTERED ATOMS:\n";
    for (auto iter = id_to_str.begin(); iter != id_to_str.end(); iter++) {
        ss << iter->first << " = " << iter->second << "\n";
    }
    BOOST_LOG_TRIVIAL(info) << ss.str();
}

Atom AtomRegistry::register_atom(const std::string& name, int id) {
    boost::lock_guard<boost::mutex> guard(mtx);

    auto res = register_atom_nolock(name, id);
    return res;
}

Atom AtomRegistry::register_atom_nolock(const std::string& name, int id) {
    auto found = id_to_str.find(id);
    if (found != id_to_str.end()) {
        if (found->second != name)
            throw Error() << "Id " << id << " already bound to atom '" << found->second << "'; cannot rebind to '" << name << "'";
        else
            return Atom(id);
    }
    auto found2 = str_to_id.find(name);
    if (found2 != str_to_id.end()) {
        if (found2->second != id)
            throw Error() << "Atom '" << name << "' already bound to id " << found2->second << "; cannot rebind to " << id;
        else
            return Atom(id);
    }

    id_to_str[id] = name;
    str_to_id[name] = id;

    return Atom(id);
}

int AtomRegistry::get_free_id() {
    for (;;) {
        auto found = id_to_str.find(next_free_id);
        if (found == id_to_str.end())
            return next_free_id;

        next_free_id++;

        if (next_free_id == std::numeric_limits<int>::max())
            throw Error() << "Exhausted all free Atom ids";
    }
}

};
