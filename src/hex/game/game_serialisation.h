#ifndef GAME_SERIALISATION_H
#define GAME_SERIALISATION_H

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
    serialiser << d.name << d.walk_cost;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, TileType& d) {
    deserialiser >> d.name >> d.walk_cost;
    return deserialiser;
}


inline Serialiser& operator<<(Serialiser& serialiser, const UnitType& d) {
    serialiser << d.name << d.moves;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, UnitType& d) {
    deserialiser >> d.name >> d.moves;
    return deserialiser;
}

/*inline std::ostream& operator<<(std::ostream& os, const Point& p) {
    return os << "(" << p.x << "," << p.y << ")";
}*/

/*static std::ostream& operator<<(std::ostream& os, const std::vector<Point> &path) {
    os << "[";
    bool first = true;
    for (std::vector<Point>::const_iterator i = path.begin(); i != path.end(); i++) {
        if (!first)
            os << ", ";
        else
            first = false;
        os << *i;
    }
    return os << "]";
}*/


#endif
