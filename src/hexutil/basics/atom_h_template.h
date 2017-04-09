#ifndef ATOM_TEMPLATE_NAME
#error "ATOM_TEMPLATE_NAME macro not defined before inclusion of atom_h_template.h"
#endif

#ifndef ATOM_TEMPLATE_INCLUDE_FILE
#error "ATOM_TEMPLATE_INCLUDE_FILE macro not defined before inclusion of atom_h_template.h"
#endif

#ifndef ATOM_TEMPLATE_START_ID
#error "ATOM_TEMPLATE_START_ID macro not defined before inclusion of atom_h_template.h"
#endif


#define PASTE2(x,y) x##y
#define CONCAT2(x,y) PASTE2(x,y)

#define PASTE3(x,y,z) x##y##z
#define CONCAT3(x,y,z) PASTE3(x,y,z)


#define ATOM_TEMPLATE_FIRST CONCAT3(First, ATOM_TEMPLATE_NAME, Atom)
#define ATOM_TEMPLATE_LAST CONCAT3(Last, ATOM_TEMPLATE_NAME, Atom)

enum CONCAT2(ATOM_TEMPLATE_NAME, Atom) {
    ATOM_TEMPLATE_FIRST = ATOM_TEMPLATE_START_ID,
#define ATOM_NAME(n) n,
#include ATOM_TEMPLATE_INCLUDE_FILE
#undef MSG_TYPE
    ATOM_TEMPLATE_LAST
};


#undef ATOM_TEMPLATE_LAST
#undef ATOM_TEMPLATE_FIRST
#undef ATOM_TEMPLATE_START_ID
#undef ATOM_TEMPLATE_INCLUDE_FILE
#undef ATOM_TEMPLATE_NAME
