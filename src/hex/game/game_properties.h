#ifndef GAME_PROPERTIES_H
#define GAME_PROPERTIES_H

#include "hexutil/basics/properties.h"


enum GameAtom {
    FirstGameAtom = 0,
    #define PROPERTY_NAME(n) n,
    #include "hex/game/property_names.h"
    #undef PROPERTY_NAME
    LastGameAtom
};


void register_property_names();

#endif
