Scripting
=========

Hex includes a *very* simple scripting language, intended to be used for defining how things
-- units, tiles, structures, UI elements -- are drawn.

Scripts are composed of terms.  A term is an object that can be:

  * A datum term, consisting of a single datum (integer, string, atom, etc.).
  * A compound term, consisting of a functor atom, and a sequence of zero or more argument terms.
  * A list term, consisting of a list of terms.

A script is generally a list term.  Each item in the list is an instruction.  The arguments to an
instruction are treated as values, with the exception that an atom term beginning with a dollar
symbol is treated as a variable dereference.

While a script is executing, it has an environment, consisting of properties from the various
objects it is being executed for.

Implementation
--------------

Interpreters for script instructions are registered on startup.  When the script executor
encounters an instruction, the appropriate interpreter is looked up in the registery and called to
execute the instruction.
