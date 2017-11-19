#include "common.h"

#include "hexutil/scripting/scripting.h"


std::string Script::signature() const {
    std::ostringstream oss;
    oss << name;
    if (parameters.size() > 0) {
        oss << '(';
        bool first = true;
        for (int i = 0; i < parameters.size(); i++) {
            if (!first)
                oss << ", ";
            oss << parameters[i];
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
        for (int i = 0; i < list_term->subterms.size(); i++) {
            const DatumTerm *datum_term = dynamic_cast<const DatumTerm *>(list_term->subterms[i].get());
            parameter_atoms.push_back(datum_term->datum);
        }
    }

    return boost::make_shared<Script>(name, parameter_atoms, std::unique_ptr<Term>(instruction));
}
