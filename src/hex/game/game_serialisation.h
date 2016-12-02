#ifndef GAME_SERIALISATION_H
#define GAME_SERIALISATION_H

#include "hex/game/combat/move.h"
#include "hex/game/game.h"


inline Serialiser& operator<<(Serialiser& serialiser, const CompressableStringVector& v) {
    std::vector<std::string> strs;
    compress_string_vector(v, strs);
    serialiser << strs;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, CompressableStringVector& v) {
    std::vector<std::string> strs;
    deserialiser >> strs;
    decompress_string_vector(strs, v);
    return deserialiser;
}

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

inline Serialiser& operator<<(Serialiser& serialiser, const Move& m) {
    serialiser.begin_tuple();
    serialiser << m.participant_id << m.target_id << m.type << m.effect;
    serialiser.end_tuple();
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, Move& m) {
    deserialiser.begin_tuple();
    deserialiser >> m.participant_id >> m.target_id >> m.type >> m.effect;
    deserialiser.end_tuple();
    return deserialiser;
}

#endif
