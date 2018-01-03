#include "common.h"

#include "hexutil/basics/properties.h"

#include "hexgame/game/game_properties.h"


namespace hex {

void register_property_names() {
#define PROPERTY_NAME(n) AtomRegistry::register_atom(#n, n);
#include "hexgame/game/property_names.h"
#undef PROPERTY_NAME
}

};
