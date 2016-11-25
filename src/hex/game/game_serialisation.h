#ifndef GAME_SERIALISATION_H
#define GAME_SERIALISATION_H

#include "hex/game/combat/move.h"
#include "hex/game/game.h"


template<typename Serialiser>
inline Serialiser& operator<<(Serialiser& serialiser, const CompressableStringVector& v) {
    std::vector<std::string> strs;
    compress_string_vector(v, strs);
    serialiser << strs;
    return serialiser;
}

template<typename Deserialiser>
inline Deserialiser& operator>>(Deserialiser& deserialiser, CompressableStringVector& v) {
    std::vector<std::string> strs;
    deserialiser >> strs;
    decompress_string_vector(strs, v);
    return deserialiser;
}

template<typename Serialiser>
class serialise_visitor: public boost::static_visitor<> {
public:
    serialise_visitor(Serialiser& serialiser): serialiser(serialiser) { }

    void operator()(const int& i) const {
        serialiser << i;
    }

    void operator()(const std::string& str) const {
        serialiser << str;
    }

private:
    Serialiser& serialiser;
};

template<typename Serialiser>
inline Serialiser& operator<<(Serialiser& serialiser, const PropertyValue& t) {
    boost::apply_visitor(serialise_visitor<Serialiser>(serialiser), t.value);
    return serialiser;
}

template<typename Deserialiser>
inline Deserialiser& operator>>(Deserialiser&deserialiser, PropertyValue& t) {
    std::string atom;
    deserialiser >> atom;
    if (deserialiser.was_quoted) {
        t.value = atom;
    } else {
        char *end;
        int intval = std::strtol(atom.c_str(), &end, 0);
        if (*end == '\0') {
            t.value = intval;
        } else {
            t.value = atom;
        }
    }
    return deserialiser;
}

template<typename Serialiser>
inline Serialiser& operator<<(Serialiser& serialiser, const Properties& p) {
    serialiser << p.data;
    return serialiser;
}

template<typename Deserialiser>
inline Deserialiser& operator>>(Deserialiser& deserialiser, Properties& p) {
    deserialiser >> p.data;
    return deserialiser;
}

template<typename Serialiser>
inline Serialiser& operator<<(Serialiser& serialiser, const IntSet& s) {
    std::set<int> set(s);
    serialiser << set;
    return serialiser;
}

template<typename Deserialiser>
inline Deserialiser& operator>>(Deserialiser& deserialiser, IntSet& s) {
    std::set<int> set;
    deserialiser >> set;
    s = IntSet(set);
    return deserialiser;
}

template<typename Serialiser>
inline Serialiser& operator<<(Serialiser& serialiser, const Point& p) {
    serialiser.begin_tuple();
    serialiser << p.x << p.y;
    serialiser.end_tuple();
    return serialiser;
}

template<typename Deserialiser>
inline Deserialiser& operator>>(Deserialiser& deserialiser, Point& p) {
    deserialiser.begin_tuple();
    deserialiser >> p.x >> p.y;
    deserialiser.end_tuple();
    return deserialiser;
}

template<typename Serialiser>
inline Serialiser& operator<<(Serialiser& serialiser, const TileType& d) {
    serialiser << d.name << d.properties;
    return serialiser;
}

template<typename Deserialiser>
inline Deserialiser& operator>>(Deserialiser& deserialiser, TileType& d) {
    deserialiser >> d.name >> d.properties;
    return deserialiser;
}

template<typename Serialiser>
inline Serialiser& operator<<(Serialiser& serialiser, const UnitType& d) {
    serialiser << d.name << d.properties;
    return serialiser;
}

template<typename Deserialiser>
inline Deserialiser& operator>>(Deserialiser& deserialiser, UnitType& d) {
    deserialiser >> d.name >> d.properties;
    return deserialiser;
}

template<typename Serialiser>
inline Serialiser& operator<<(Serialiser& serialiser, const StructureType& d) {
    serialiser << d.name << d.properties;
    return serialiser;
}

template<typename Deserialiser>
inline Deserialiser& operator>>(Deserialiser& deserialiser, StructureType& d) {
    deserialiser >> d.name >> d.properties;
    return deserialiser;
}

template<typename Serialiser>
inline Serialiser& operator<<(Serialiser& serialiser, const Move& m) {
    serialiser.begin_tuple();
    serialiser << m.participant_id << m.target_id << m.type << m.effect;
    serialiser.end_tuple();
    return serialiser;
}

template<typename Deserialiser>
inline Deserialiser& operator>>(Deserialiser& deserialiser, Move& m) {
    deserialiser.begin_tuple();
    deserialiser >> m.participant_id >> m.target_id >> m.type >> m.effect;
    deserialiser.end_tuple();
    return deserialiser;
}

#endif
