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

void Execution::execute_script(Script *script) {
    execute_instruction(script->instructions.get());
    return_active = false;
}

void Execution::execute_instruction(const Term *script_term) {
    const Interpreter *interpreter = InterpreterRegistry::get_interpreter(script_term);
    if (interpreter) {
        try {
            interpreter->execute(script_term, this);
        } catch (const boost::bad_get& err) {
            throw ScriptError() << "Exception in instruction defined in line " << script_term->line_no << ": " << script_term;
        }
        return;
    }

    throw ScriptError() << "Can't find interpreter for term: " << script_term << "\n";
}


const Datum& Execution::get(const Atom& name) const {
    if (variables.contains(name))
        return variables.get(name);
    for (auto iter = properties_list.begin(); iter != properties_list.end(); iter++) {
        Properties& properties = **iter;
        if (properties.contains(name))
            return properties.get(name);
    }
    return variables.get(name);
}

const Datum& Execution::get_argument(const Term *term, int position) {
    const CompoundTerm *script_term = dynamic_cast<const CompoundTerm *>(term);
    if (!script_term) {
        throw ScriptError() << "Cannot get argument from non-list term!";
    }
    if (position >= script_term->subterms.size()) {
        throw ScriptError() << boost::format("Cannot get argument %d from term with %d arguments!") % position % script_term->subterms.size();
    }

    const Term *subterm = script_term->subterms[position].get();
    const DatumTerm *arg_term = dynamic_cast<const DatumTerm *>(subterm);
    if (!arg_term) {
        throw ScriptError() << "Argument is not a datum!";
    }
    const Datum *datum_ptr = &arg_term->datum;
    while (datum_ptr->is<Atom>()) {
        const std::string& atom_name = static_cast<const std::string>(datum_ptr->get<Atom>());
        if (atom_name.at(0) != '$')
            break;
        const char *var_str = atom_name.c_str() + 1;
        datum_ptr = &get(static_cast<Atom>(var_str));
    }
    return *datum_ptr;
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


std::map<Atom, std::unique_ptr<Interpreter> > InterpreterRegistry::interpreters;
