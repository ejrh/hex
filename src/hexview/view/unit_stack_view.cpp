#include "common.h"

#include "hexview/view/view.h"


namespace hex {

void UnitStackView::set_representative(ViewResources *resources) {
    representative = 0;
    for (unsigned int i = 1; i < stack->units.size(); i++) {
        if (stack->units[i]->has_property(Transport)) {
            representative = i;
            break;
        }
        //TODO pick most valuable
    }
    if (representative < stack->units.size()) {
        UnitType& unit_type = *stack->units[representative]->type;
        view_def = resources->get_unit_view_def(unit_type.name);
    } else {
        view_def.reset();
    }
}

};
