#ifndef PLAYER_H
#define PLAYER_H

class Faction;

class Player {
public:
    Player(int id, const std::string& name);
    void grant_view(Faction *faction, bool allow);
    void grant_control(Faction *faction, bool allow);
    bool has_view(Faction *faction) const;
    bool has_control(Faction *faction) const;

public:
    int id;
    std::string name;

private:
    std::set<Faction *> faction_view;
    std::set<Faction *> faction_control;
};


#endif
