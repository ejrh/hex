#ifndef PLAYER_H
#define PLAYER_H

#include "hexgame/game/game.h"


namespace hex {

class Player {
public:
    Player(int id, const std::string& name);
    void grant_view(Faction::pointer faction, bool allow);
    void grant_control(Faction::pointer faction, bool allow);
    bool has_view(Faction::pointer faction) const;
    bool has_control(Faction::pointer faction) const;

public:
    int id;
    std::string name;

private:
    std::unordered_set<int> faction_view;
    std::unordered_set<int> faction_control;
};

};

#endif
