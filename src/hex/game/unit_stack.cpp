#include "common.h"

#include "hex/game/game.h"

void UnitStack::transfer_units(std::set<int> unit_selection, UnitStack *target_stack) {
    std::vector<Unit *>::iterator iter = units.begin();
    int i = 0;
    while (iter != units.end()) {
        if (unit_selection.find(i) != unit_selection.end()) {
            target_stack->units.push_back(*iter);
            iter = units.erase(iter);
        } else {
            i++;
            iter++;
        }
    }
}
