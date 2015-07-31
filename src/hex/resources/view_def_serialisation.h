#ifndef VIEW_DATA_SERIALISATION
#define VIEW_DATA_SERIALISATION

inline Serialiser& operator<<(Serialiser& serialiser, const FactionViewDef& d) {
    serialiser << d.name << d.r << d.g << d.b;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, FactionViewDef& d) {
    deserialiser >> d.name >> d.r >> d.g >> d.b;
    return deserialiser;
}

inline Serialiser& operator<<(Serialiser& serialiser, const TileViewDef& d) {
    serialiser << d.name;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, TileViewDef& d) {
    deserialiser >> d.name;
    return deserialiser;
}

inline Serialiser& operator<<(Serialiser& serialiser, const UnitViewDef& d) {
    serialiser << d.name << d.hold_bpm << d.move_bpm << d.move_speed;
    return serialiser;
}

inline Deserialiser& operator>>(Deserialiser& deserialiser, UnitViewDef& d) {
    deserialiser >> d.name >> d.hold_bpm >> d.move_bpm >> d.move_speed;
    return deserialiser;
}

#endif
