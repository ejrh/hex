#ifndef SCRIPTING_H
#define SCRIPTING_H

#include "hexutil/basics/error.h"
#include "hexutil/basics/objmap.h"
#include "hexutil/basics/properties.h"
#include "hexutil/basics/term.h"


namespace hex {

class Execution;

class ScriptError: public BaseError<ScriptError> {
};

class Interpreter {
public:
    Interpreter(Atom functor): functor(functor) { }
    virtual ~Interpreter() { }

    virtual Datum execute(const Term *instruction, Execution *execution) const = 0;

public:
    Atom functor;
};

class Script: public boost::enable_shared_from_this<Script> {
public:
    typedef boost::shared_ptr<Script> pointer;

    Script(const std::string& name, std::unique_ptr<Term> instructions):
            name(name), instructions(std::move(instructions)) { }
    Script(const std::string& name, const std::vector<Atom>& parameters, std::unique_ptr<Term> instructions):
            name(name), parameters(parameters), instructions(std::move(instructions)) { }

    std::string signature() const;

public:
    std::string name;
    std::vector<Atom> parameters;
    std::unique_ptr<Term> instructions;
};

class Execution {
public:
    Execution(StrMap<Script> *scripts): scripts(scripts), return_active(false) { }
    virtual ~Execution() { }

    void add_properties(Properties *properties);

    void run(Script *script);
    void run(const std::string& script_name);

    Datum execute_script(Script *script);
    Datum execute_instruction(const Term *script_term);

    const Datum& get(const Atom& name) const;
    const Datum get_argument(const Term *term, unsigned int position);
    const Term *get_subterm(const Term *term, unsigned int position);
    std::vector<int> get_as_intvector(const Term *term, unsigned int position);

    template<typename T>
    const T& get_as(const Atom& name) const {
        try {
            return get(name).get<T>();
        } catch (const boost::bad_get& err) {
            throw ScriptError() << boost::format("Can't get variable %s as specified type (value is %s): %s") % name % get(name) % err.what();
        }
    }

public:
    StrMap<Script> *scripts;
    Properties locals;
    Properties variables;
    std::vector<Properties *> properties_list;
    bool return_active;
};


class PushLocals {
public:
    PushLocals(Execution& execution, Properties& locals):
            execution(execution), locals(locals) {
        std::swap(execution.locals, locals);
    }
    ~PushLocals() {
        std::swap(execution.locals, locals);
    }

private:
    Execution& execution;
    Properties& locals;
};


class InterpreterRegistry {
public:
    static void register_interpreter(Interpreter *interpreter) {
        interpreters[interpreter->functor] = std::unique_ptr<Interpreter>(interpreter);
    }

    static Interpreter *get_interpreter(const Term *script_term) {
        const CompoundTerm *instr_term = dynamic_cast<const CompoundTerm *>(script_term);
        if (!instr_term)
            return nullptr;
        Atom functor = instr_term->functor;
        auto found = interpreters.find(functor);
        if (found == interpreters.end())
            return nullptr;
        return found->second.get();
    }

private:
    static std::unordered_map<Atom, std::unique_ptr<Interpreter> > interpreters;
};

void register_builtin_interpreters();

Script::pointer compile_script(const std::string& name, Term *parameters, Term *instruction);

};

#endif
