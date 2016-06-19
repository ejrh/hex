#ifndef MOVE_H
#define MOVE_H

class Move {
public:
    Move():
        participant_id(-1), target_id(-1), type(UnknownPropertyType), damage(0) { }
    Move(int participant_id, int target_id, PropertyType type, int damage):
        participant_id(participant_id), target_id(target_id), type(type), damage(damage) { }

public:
    int participant_id;
    int target_id;
    PropertyType type;
    int damage;
};

inline std::ostream& operator<<(std::ostream& os, const Move& a) {
    os << boost::format("%d-%d, type %s damage %d") % a.participant_id % a.target_id % get_property_type_name(a.type) % a.damage;
    return os;
}

#endif
