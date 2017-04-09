#ifndef SCRIPTING_H
#define SCRIPTING_H

#include "hexutil/basics/error.h"
#include "hexutil/basics/objmap.h"
#include "hexutil/basics/properties.h"
#include "hexutil/basics/term.h"


class Execution;

class Interpreter {
public:
    Interpreter(Atom functor): functor(functor) { }
    virtual ~Interpreter() { }

    virtual void execute(const Term *instruction, Execution *execution) const = 0;

public:
    Atom functor;
};

class Script: public boost::enable_shared_from_this<Script> {
public:
    typedef boost::shared_ptr<Script> pointer;

    Script(const std::string& name, std::unique_ptr<Term> instructions):
            name(name), instructions(std::move(instructions)) { }

public:
    std::string name;
    std::unique_ptr<Term> instructions;
};

class Execution {
public:
    Execution(StrMap<Script> *scripts): scripts(scripts), return_active(false) { }
    virtual ~Execution() { }

    void add_properties(Properties *properties);

    void run(Script *script);
    void run(const std::string& script_name);

    void execute_script(Script *script);
    void execute_instruction(const Term *script_term);

    const Datum& get(const Atom& name) const;
    const Datum& get_argument(const Term *term, int position);
    const Term *get_subterm(const Term *term, int position);

public:
    StrMap<Script> *scripts;
    Properties variables;
    std::vector<Properties *> properties_list;
    bool return_active;
};

class ScriptError: public BaseError<ScriptError> {
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
    static std::map<Atom, std::unique_ptr<Interpreter> > interpreters;
};

void register_builtin_interpreters();


#endif
