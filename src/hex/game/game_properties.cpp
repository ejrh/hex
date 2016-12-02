#include "common.h"

#include "hexutil/basics/properties.h"
#include "hex/game/game_properties.h"


void register_property_names() {
#define PROPERTY_NAME(n) AtomRegistry::register_atom(#n, n);
#include "hex/game/property_names.h"
#undef PROPERTY_NAME
}
