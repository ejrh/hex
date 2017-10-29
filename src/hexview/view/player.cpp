#include "common.h"

#include "hexview/view/player.h"

Player::Player(int id, const std::string& name): id(id), name(name) {
}

void Player::grant_view(Faction::pointer faction, bool allow) {
    if (allow)
        faction_view.insert(faction->id);
    else
        faction_view.erase(faction->id);
}

void Player::grant_control(Faction::pointer faction, bool allow) {
    if (allow)
        faction_control.insert(faction->id);
    else
        faction_control.erase(faction->id);
}

bool Player::has_view(Faction::pointer faction) const {
    return faction_view.find(faction->id) != faction_view.end();
}

bool Player::has_control(Faction::pointer faction) const {
    return faction_control.find(faction->id) != faction_control.end();
}
