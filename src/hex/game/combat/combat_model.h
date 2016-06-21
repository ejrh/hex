#ifndef COMBAT_MODEL
#define COMBAT_MODEL

#include "hex/game/properties.h"

class Battle;
class Participant;
class MoveType;

class CombatModel {
public:
    static CombatModel *get_default() {
        return &default_combat_model;
    }

    CombatModel() {
        populate_move_types();
    }

    std::vector<const MoveType *> get_available_move_types(const Battle& battle, const Participant& participant) const;

    const MoveType *get_move_type(const Move& move) const;

private:
    void populate_move_types();

private:
    std::map<PropertyType, MoveType *> move_types;
    static CombatModel default_combat_model;
};

#endif
