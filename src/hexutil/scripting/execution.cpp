#include "common.h"

#include "hexutil/basics/objmap.h"
#include "hexutil/scripting/scripting.h"


void Execution::add_properties(Properties *properties) {
    properties_list.push_back(properties);
}

void Execution::run(Script *script) {
    execute_script(script);
}

void Execution::run(const std::string& script_name) {
    Script *script = scripts->get(script_name).get();
    execute_script(script);
}

Datum Execution::execute_script(Script *script) {
    Datum rv = execute_instruction(script->instructions.get());
    return_active = false;
    return rv;
}

Datum Execution::execute_instruction(const Term *script_term) {
    const Interpreter *interpreter = InterpreterRegistry::get_interpreter(script_term);
    if (interpreter) {
        Datum rv;
        try {
            rv = interpreter->execute(script_term, this);
        } catch (const boost::bad_get& err) {
            throw ScriptError() << "Line " << script_term->line_no << " '" << script_term << "': " << err.what();
        }
        return rv;
    }

    throw ScriptError() << "Can't find interpreter for term: " << script_term << "\n";
}

const Datum& Execution::get(const Atom& name) const {
    if (locals.contains(name))
        return locals.get(name);
    if (variables.contains(name))
        return variables.get(name);
    for (auto iter = properties_list.begin(); iter != properties_list.end(); iter++) {
        Properties& properties = **iter;
        if (properties.contains(name))
            return properties.get(name);
    }
    return variables.get(name);
}

const Datum Execution::get_argument(const Term *term, int position) {
    const CompoundTerm *script_term = dynamic_cast<const CompoundTerm *>(term);
    if (!script_term) {
        throw ScriptError() << "Cannot get argument from non-list term!";
    }
    if (position >= script_term->subterms.size()) {
        throw ScriptError() << boost::format("Cannot get argument %d from term with %d arguments!") % position % script_term->subterms.size();
    }

    const Term *subterm = script_term->subterms[position].get();

    // If it's a compound term, it needs to be executed and its result used
    const CompoundTerm *compound_term = dynamic_cast<const CompoundTerm *>(subterm);
    if (compound_term) {
        Datum rv = execute_instruction(compound_term);
        return rv;
    }

    const DatumTerm *arg_term = dynamic_cast<const DatumTerm *>(subterm);
    if (!arg_term) {
        throw ScriptError() << "Argument is not a datum!";
    }
    Datum datum = arg_term->datum;
    while (datum.is<Atom>()) {
        const std::string& atom_name = static_cast<const std::string>(datum.get<Atom>());
        if (atom_name.empty() || atom_name.at(0) != '$')
            break;
        const char *var_str = atom_name.c_str() + 1;
        datum = get(static_cast<Atom>(var_str));
    }
    return datum;
}

const Term *Execution::get_subterm(const Term *term, int position) {
    const CompoundTerm *script_term = dynamic_cast<const CompoundTerm *>(term);
    if (!script_term) {
        throw ScriptError() << "Cannot get argument from non-list term!";
    }
    if (position >= script_term->subterms.size()) {
        throw ScriptError() << boost::format("Cannot get argument %d from term with %d arguments!") % position % script_term->subterms.size();
    }

    return script_term->subterms[position].get();
}

std::vector<int> Execution::get_as_intvector(const Term *term, int position) {
    const CompoundTerm *argterm = dynamic_cast<const CompoundTerm *>(get_subterm(term, position));
    std::vector<int> result;
    for (int i = 0; i < argterm->subterms.size(); i++) {
        int value = get_argument(argterm, i);
        result.push_back(value);
    }
    return result;
}


std::unordered_map<Atom, std::unique_ptr<Interpreter> > InterpreterRegistry::interpreters;
