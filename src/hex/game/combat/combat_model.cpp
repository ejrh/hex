#include "common.h"

#include "hex/game/game.h"
#include "hex/game/combat/combat.h"
#include "hex/game/combat/combat_model.h"
#include "hex/game/combat/move_types.h"

CombatModel CombatModel::default_combat_model;

std::vector<const MoveType *> CombatModel::get_available_move_types(const Battle& battle, const Participant& participant) const {
    std::vector<const MoveType *> types;

    for (std::map<PropertyType, MoveType>::const_iterator iter = move_types.begin(); iter != move_types.end(); iter++) {
        PropertyType type = iter->first;
        const MoveType& move_type = iter->second;
        if (participant.unit->has_property(type)) {
            types.push_back(&move_type);
        }
    }
    return types;
}

const MoveType& CombatModel::get_move_type(const Move& move) const {
    std::map<PropertyType, MoveType>::const_iterator iter = move_types.find(move.type);
    if (iter == move_types.end()) {
        throw Error() << "Cannot find move type for " << get_property_type_name(move.type);
    }

    return iter->second;
}

void CombatModel::populate_move_types() {
    move_types[Archery] = ArcheryMoveType();
    move_types[Charge] = ChargeMoveType();
    move_types[Healing] = HealingMoveType();
    move_types[Strike] = StrikeMoveType();
}
