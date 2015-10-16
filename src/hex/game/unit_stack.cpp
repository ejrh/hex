#include "common.h"

#include "hex/game/game.h"

void UnitStack::transfer_units(const IntSet unit_selection, UnitStack& target_stack) {
    std::vector<Unit::pointer>::iterator iter = units.begin();
    int i = 0;
    while (iter != units.end()) {
        if (unit_selection.contains(i)) {
            target_stack.units.push_back(*iter);
            iter = units.erase(iter);
        } else {
            iter++;
        }
        i++;
    }
}

bool UnitStack::has_units(const IntSet unit_selection) const {
    if (unit_selection.last() >= units.size())
        return false;

    return true;
}
