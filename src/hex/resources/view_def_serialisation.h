#ifndef VIEW_DATA_SERIALISATION
#define VIEW_DATA_SERIALISATION

#include "hex/messaging/serialiser.h"
#include "hex/resources/view_def.h"


inline Serialiser& operator<<(Serialiser& serialiser, const FactionViewDef& d) {
    serialiser << d.name << d.r << d.g << d.b;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, FactionViewDef& d) {
    deserialiser >> d.name >> d.r >> d.g >> d.b;
    return deserialiser;
}

inline Serialiser& operator<<(Serialiser& serialiser, const TileViewDef& d) {
    serialiser << d.name << d.r << d.g << d.b;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, TileViewDef& d) {
    deserialiser >> d.name >> d.r >> d.g >> d.b;
    return deserialiser;
}

inline Serialiser& operator<<(Serialiser& serialiser, const TransitionDef& d) {
    serialiser << d.dirs << d.type_names << d.images;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, TransitionDef& d) {
    deserialiser >> d.dirs >> d.type_names >> d.images;
    return deserialiser;
}

inline Serialiser& operator<<(Serialiser& serialiser, const FeatureDef& d) {
    serialiser << d.centre_x << d.centre_y << d.images;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, FeatureDef& d) {
    deserialiser >> d.centre_x >> d.centre_y >> d.images;
    return deserialiser;
}

inline Serialiser& operator<<(Serialiser& serialiser, const UnitViewDef& d) {
    serialiser << d.name << d.move_speed;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, UnitViewDef& d) {
    deserialiser >> d.name >> d.move_speed;
    return deserialiser;
}

inline Serialiser& operator<<(Serialiser& serialiser, const StructureViewDef& d) {
    serialiser << d.name;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, StructureViewDef& d) {
    deserialiser >> d.name;
    return deserialiser;
}

#endif
