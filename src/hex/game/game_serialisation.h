#ifndef GAME_SERIALISATION_H
#define GAME_SERIALISATION_H

#include "hex/game/combat/move.h"
#include "hex/game/game.h"
#include "hex/messaging/serialiser.h"


inline Serialiser& operator<<(Serialiser& serialiser, const IntSet& s) {
    std::set<int> set(s);
    serialiser << set;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, IntSet& s) {
    std::set<int> set;
    deserialiser >> set;
    s = IntSet(set);
    return deserialiser;
}


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
    serialiser << d.name << d.properties;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, UnitType& d) {
    deserialiser >> d.name >> d.properties;
    return deserialiser;
}


inline Serialiser& operator<<(Serialiser& serialiser, const StructureType& d) {
    serialiser << d.name << d.properties;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, StructureType& d) {
    deserialiser >> d.name >> d.properties;
    return deserialiser;
}


inline Serialiser& operator<<(Serialiser& serialiser, const PropertyType& t) {
    serialiser << get_property_type_name(t);
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, PropertyType& t) {
    std::string name;
    deserialiser >> name;
    t = get_property_type(name);
    return deserialiser;
}

inline Serialiser& operator<<(Serialiser& serialiser, const Move& m) {
    serialiser.begin_tuple();
    serialiser << m.participant_id << m.target_id << m.damage;
    serialiser.end_tuple();
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, Move& m) {
    deserialiser.begin_tuple();
    deserialiser >> m.participant_id >> m.target_id >> m.damage;
    deserialiser.end_tuple();
    return deserialiser;
}


#endif
