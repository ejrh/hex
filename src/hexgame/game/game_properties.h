#ifndef GAME_PROPERTIES_H
#define GAME_PROPERTIES_H

#include "hexutil/basics/properties.h"


namespace hex {

enum GameAtom {
    FirstGameAtom = 2000,
    #define PROPERTY_NAME(n) n,
    #include "hexgame/game/property_names.h"
    #undef PROPERTY_NAME
    LastGameAtom
};


void register_property_names();

};

#endif
