#ifndef MOVE_H
#define MOVE_H

class Move {
public:
    Move():
        participant_id(-1), target_id(-1), type(0), effect(0) { }
    Move(int participant_id, int target_id, Atom type, int effect):
        participant_id(participant_id), target_id(target_id), type(type), effect(effect) { }

public:
    int participant_id;
    int target_id;
    Atom type;
    int effect;
};

inline std::ostream& operator<<(std::ostream& os, const Move& a) {
    os << boost::format("%d-%d, type %s effect %d") % a.participant_id % a.target_id % a.type % a.effect;
    return os;
}

#endif
