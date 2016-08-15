#include "common.h"

#include "hex/game/game.h"
#include "hex/game/combat/combat.h"
#include "hex/game/combat/combat_model.h"
#include "hex/game/combat/move_types.h"

CombatModel CombatModel::default_combat_model;

CombatModel::~CombatModel() {
    for (std::map<PropertyName, MoveType *>::iterator iter = move_types.begin(); iter != move_types.end(); iter++) {
        delete iter->second;
    }
}

std::vector<const MoveType *> CombatModel::get_available_move_types(const Battle& battle, const Participant& participant) const {
    std::vector<const MoveType *> types;

    for (std::map<PropertyName, MoveType *>::const_iterator iter = move_types.begin(); iter != move_types.end(); iter++) {
        PropertyName type = iter->first;
        const MoveType *move_type = iter->second;
        if (participant.unit->get_property<int>(type) > 0) {
            types.push_back(move_type);
        }
    }
    return types;
}

const MoveType *CombatModel::get_move_type(const Move& move) const {
    std::map<PropertyName, MoveType *>::const_iterator iter = move_types.find(move.type);
    if (iter == move_types.end()) {
        throw Error() << "Cannot find move type for " << get_property_name_str(move.type);
    }

    return iter->second;
}

void CombatModel::populate_move_types() {
    move_types[Archery] = new ArcheryMoveType();
    move_types[Charge] = new ChargeMoveType();
    move_types[Healing] = new HealingMoveType();
    move_types[Strike] = new StrikeMoveType();
    move_types[Riposte] = new RiposteMoveType();
}
