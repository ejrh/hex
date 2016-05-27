#include "common.h"

#include "hex/game/game.h"
#include "hex/game/combat/combat.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"
#include "hex/view/level_renderer.h"
#include "hex/view/view.h"
#include "hex/view/combat/combat_view.h"
#include "hex/view/combat/battle_window.h"


BattleWindow::BattleWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, BattleView *view, LevelRenderer *renderer):
        UiWindow(x, y, width, height), resources(resources), graphics(graphics), view(view), renderer(renderer) {
}

bool BattleWindow::receive_event(SDL_Event *evt) {
    return false;
}

void BattleWindow::draw() {
    for (int i = 0; i <= 7; i++) {
        draw_stack(i);
    }
}

void BattleWindow::draw_stack(int stack_num) {
    BattleStackView& stack_view = view->battle_stack_views[stack_num];

    graphics->fill_rectangle(200,100,100, stack_view.x, stack_view.y, BattleView::battle_stack_width, BattleView::battle_stack_height);

    for (std::vector<ParticipantView>::iterator iter = view->participant_views.begin(); iter != view->participant_views.end(); iter++) {
        ParticipantView& pv = *iter;
        int highlight = 0;
        Unit& unit = *pv.participant->unit;
        UnitViewDef& view_def = *resources->get_unit_view_def(unit.type->name);
        renderer->draw_unit(pv.x, pv.y, unit, view_def, highlight);
    }
}
