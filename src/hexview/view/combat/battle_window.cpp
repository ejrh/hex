#include "common.h"

#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"

#include "hexgame/game/game.h"
#include "hexgame/game/combat/combat.h"

#include "hexview/view/unit_renderer.h"
#include "hexview/view/view.h"
#include "hexview/view/combat/combat_view.h"
#include "hexview/view/combat/battle_window.h"


BattleWindow::BattleWindow(int x, int y, int width, int height, Resources *resources, Graphics *graphics, BattleView *view, UnitRenderer *renderer):
        UiWindow(x, y, width, height, WindowIsVisible),
        resources(resources), graphics(graphics), view(view), renderer(renderer) {
    target_images = resources->image_series["TARGETS"];
}

void BattleWindow::draw(const UiContext& context) {
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

        if (pv.selected) {
            Image *target = target_images[0].image;
            if (target != NULL) {
                graphics->blit(target, pv.x - target->width / 2, pv.y - target->height, SDL_BLENDMODE_BLEND, 128);
            }
        }

        Unit& unit = *pv.participant->unit;
        renderer->draw_unit(pv.x, pv.y, pv);


        if (pv.selected) {
            Move& move = view->battle->moves[view->current_move];
            TextFormat tf(SmallFont14, true, 250, 250, 250);
            tf.write_text(graphics, move.type, pv.x, pv.y + 20);
        }

        if (pv.targetted) {
            Image *target = target_images[1].image;
            if (target != NULL) {
                graphics->blit(target, pv.x - target->width / 2, pv.y - target->height, SDL_BLENDMODE_BLEND);
            }
        }

        if (pv.participant->is_alive()) {
            graphics->fill_rectangle(0, 0, 0, pv.x - 21, pv.y - 43, 42, 5);
            float health = pv.participant->get_health() / (float) pv.participant->get_max_health();
            float health2 = (pv.participant->get_health() - 1) / (float) (pv.participant->get_max_health() - 1);
            int r = (1.0 - health2) * 255;
            int g = health2 * 255;
            int b = 0;
            int w = health * 40;
            graphics->fill_rectangle(r, g, b, pv.x - 20, pv.y - 42, w, 3);
        }
    }
}
