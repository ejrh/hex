#include "common.h"

#include "hexutil/scripting/scripting.h"


namespace hex {

std::string Script::signature() const {
    std::ostringstream oss;
    oss << name;
    if (parameters.size() > 0) {
        oss << '(';
        bool first = true;
        for (auto iter = parameters.begin(); iter != parameters.end(); iter++) {
            if (!first)
                oss << ", ";
            oss << *iter;
            first = false;
        }
        oss << ')';
    }
    return oss.str();
}


Script::pointer compile_script(const std::string& name, Term *parameters, Term *instruction) {
    std::vector<Atom> parameter_atoms;

    if (instruction == nullptr) {
        instruction = parameters;
    } else {
        const CompoundTerm *list_term = dynamic_cast<const CompoundTerm *>(parameters);
        for (unsigned int i = 0; i < list_term->subterms.size(); i++) {
            const DatumTerm *datum_term = dynamic_cast<const DatumTerm *>(list_term->subterms[i].get());
            parameter_atoms.push_back(datum_term->datum);
        }
    }

    return boost::make_shared<Script>(name, parameter_atoms, std::unique_ptr<Term>(instruction));
}

};
