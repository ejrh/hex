#include "common.h"

#include "hexutil/basics/atom.h"

std::map<int, std::string> AtomRegistry::id_to_str;
std::map<std::string, int> AtomRegistry::str_to_id;
int AtomRegistry::next_free_id(1);
const std::string AtomRegistry::empty_string;

Atom AtomRegistry::register_atom(const std::string& name, int id) {
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

    if (id >= next_free_id)
        next_free_id = id + 1;

    return Atom(id);
}
