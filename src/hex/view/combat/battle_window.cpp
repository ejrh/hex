#include "common.h"

#include "hex/game/game.h"
#include "hex/game/combat/combat.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"
#include "hex/view/unit_renderer.h"
#include "hex/view/view.h"
#include "hex/view/combat/combat_view.h"
#include "hex/view/combat/battle_window.h"


BattleWindow::BattleWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, BattleView *view, UnitRenderer *renderer):
        UiWindow(x, y, width, height), resources(resources), graphics(graphics), view(view), renderer(renderer) {
}

bool BattleWindow::receive_event(SDL_Event *evt) {
    return false;
}

void BattleWindow::draw() {
    view->update();

    for (int i = 0; i <= 6; i++) {
        draw_stack(i);
    }
}

void BattleWindow::draw_stack(int stack_num) {
    BattleStackView& stack_view = view->battle_stack_views[stack_num];

    graphics->fill_rectangle(200,100,100, stack_view.x, stack_view.y, BattleView::battle_stack_width, BattleView::battle_stack_height);

    for (int i = 0; i < 12; i++) {
        if (stack_view.participants[i] == -1)
            continue;

        ParticipantView& pv = view->participant_views[stack_view.participants[i]];
        Unit& unit = *pv.participant->unit;
        renderer->draw_unit(pv.x, pv.y, pv);

        if (pv.selected) {
            Move& move = view->battle->moves[view->current_move];
            TextFormat tf(graphics, SmallFont14, true, 250, 250, 250);
            tf.write_text(get_property_type_name(move.type), pv.x, pv.y + 20);
        }

        if (pv.participant->is_alive()) {
            graphics->fill_rectangle(0, 0, 0, pv.x - 21, pv.y - 43, 42, 5);
            float health = unit.get_property(Health) / (float) unit.type->get_property(Health);
            float health2 = (unit.get_property(Health) - 1) / (float) (unit.type->get_property(Health) - 1);
            int r = (1.0 - health2) * 255;
            int g = health2 * 255;
            int b = 0;
            int w = health * 40;
            graphics->fill_rectangle(r, g, b, pv.x - 20, pv.y - 42, w, 3);
        }
    }
}
