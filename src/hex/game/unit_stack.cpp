#include "common.h"

#include "hex/game/game.h"

UnitStack::pointer UnitStack::copy_subset(const IntSet unit_selection) const {
    UnitStack::pointer new_stack = boost::make_shared<UnitStack>(position, owner);
    for (unsigned int i = 0; i < units.size(); i++) {
        if (unit_selection.contains(i))
            new_stack->units.push_back(boost::make_shared<Unit>(*units[i]));
    }
    return new_stack;
}

void UnitStack::transfer_units(const IntSet unit_selection, UnitStack& target_stack) {
    auto iter = units.begin();
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
