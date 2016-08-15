#ifndef MOVE_H
#define MOVE_H

class Move {
public:
    Move():
        participant_id(-1), target_id(-1), type(UnknownPropertyName), effect(0) { }
    Move(int participant_id, int target_id, PropertyName type, int effect):
        participant_id(participant_id), target_id(target_id), type(type), effect(effect) { }

public:
    int participant_id;
    int target_id;
    PropertyName type;
    int effect;
};

inline std::ostream& operator<<(std::ostream& os, const Move& a) {
    os << boost::format("%d-%d, type %s effect %d") % a.participant_id % a.target_id % get_property_name_str(a.type) % a.effect;
    return os;
}

#endif
