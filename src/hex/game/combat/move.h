#ifndef MOVE_H
#define MOVE_H

class Move {
public:
    Move():
        participant_id(-1), target_id(-1), damage(0) { }
    Move(int participant_id, int target_id, int damage):
        participant_id(participant_id), target_id(target_id), damage(damage) { }

public:
    int participant_id;
    int target_id;
    int damage;
};

inline std::ostream& operator<<(std::ostream& os, const Move& a) {
    os << boost::format("%d-%d, damage %d") % a.participant_id % a.target_id % a.damage;
    return os;
}

#endif
