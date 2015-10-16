#ifndef PLAYER_H
#define PLAYER_H

#include "hex/game/game.h"


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
    std::set<Faction::pointer> faction_view;
    std::set<Faction::pointer> faction_control;
};


#endif
