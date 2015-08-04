#ifndef GAME_SERIALISATION_H
#define GAME_SERIALISATION_H

#include "hex/game/game.h"
#include "hex/messaging/serialiser.h"


inline Serialiser& operator<<(Serialiser& serialiser, const Point& p) {
    serialiser.begin_tuple();
    serialiser << p.x << p.y;
    serialiser.end_tuple();
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, Point& p) {
    deserialiser.begin_tuple();
    deserialiser >> p.x >> p.y;
    deserialiser.end_tuple();
    return deserialiser;
}


inline Serialiser& operator<<(Serialiser& serialiser, const TileType& d) {
    serialiser << d.name << d.properties;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, TileType& d) {
    deserialiser >> d.name >> d.properties;
    return deserialiser;
}


inline Serialiser& operator<<(Serialiser& serialiser, const UnitType& d) {
    serialiser << d.name << d.abilities << d.moves << d.sight;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, UnitType& d) {
    deserialiser >> d.name >> d.abilities >> d.moves >> d.sight;
    return deserialiser;
}


inline Serialiser& operator<<(Serialiser& serialiser, const TraitType& t) {
    serialiser << get_trait_type_name(t);
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, TraitType& t) {
    std::string name;
    deserialiser >> name;
    t = get_trait_type(name);
    return deserialiser;
}


#endif
