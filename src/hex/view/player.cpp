#include "common.h"

#include "hex/view/player.h"

Player::Player(int id, const std::string& name): id(id), name(name) {
}

void Player::grant_view(Faction *faction, bool allow) {
    if (allow)
        faction_view.insert(faction);
    else
        faction_view.erase(faction);
}

void Player::grant_control(Faction *faction, bool allow) {
    if (allow)
        faction_control.insert(faction);
    else
        faction_control.erase(faction);
}

bool Player::has_view(Faction *faction) const {
    return faction_view.find(faction) != faction_view.end();
}

bool Player::has_control(Faction *faction) const {
    return faction_control.find(faction) != faction_control.end();
}
