#include "common.h"

#include "hexutil/basics/hexgrid.h"

#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"

#include "hexgame/game/game.h"
#include "hexgame/game/combat/combat.h"

#include "hexview/view/unit_renderer.h"
#include "hexview/view/view.h"
#include "hexview/view/combat/combat_view.h"
#include "hexview/view/combat/combat_window.h"


CombatStackWindow::CombatStackWindow(int x, int y, int width, int height, ViewResources *resources, CombatView *view, UnitRenderer *renderer, int stack_num):
        UiWindow(x, y, width, height, WindowIsVisible),
        resources(resources), view(view), renderer(renderer), stack_num(stack_num) {
    target_images = resources->image_series["TARGETS"];

    for (int i = 0; i < MAX_UNITS; i++) {
        unit_x[i] = (i % 4) * CombatWindow::participant_width + CombatWindow::participant_width / 2;
        unit_y[i] = (i  / 4) * CombatWindow::participant_height + CombatWindow::participant_height / 2;
    }
}

void CombatStackWindow::draw(const UiContext& context) {
    CombatStackView& stack_view = view->combat_stack_views[stack_num];

    context.fill_rectangle(200, 100, 100, 0, 0, width, height);

    for (int i = 0; i < 12; i++) {
        if (stack_view.participants[i] == -1)
            continue;

        int px = context.translate_x + unit_x[i];
        int py = context.translate_y + unit_y[i];

        ParticipantView& pv = view->participant_views[stack_view.participants[i]];

        if (pv.selected) {
            Image *target = target_images[0].image;
            if (target != NULL) {
                context.graphics->blit(target, px - target->width / 2, py - target->height / 2, SDL_BLENDMODE_BLEND, 128);
            }
        }

        Unit& unit = *pv.participant->unit;
        renderer->draw_unit(px, py, pv);


        if (pv.selected) {
            Move& move = view->battle->moves[view->current_move];
            TextFormat tf(SmallFont14, true, 250, 250, 250);
            tf.write_text(context.graphics, move.type, px, py + CombatWindow::participant_height / 2 + 8);
        }

        if (pv.targetted) {
            Image *target = target_images[1].image;
            if (target != NULL) {
                context.graphics->blit(target, px - target->width / 2, py - target->height / 2, SDL_BLENDMODE_BLEND);
            }
        }

        if (pv.participant->is_alive()) {
            renderer->draw_health_bar(px - CombatWindow::participant_width / 2 + 8, py - CombatWindow::participant_height / 2 + 8, CombatWindow::participant_width - 16, 5, pv.participant->get_health(), pv.participant->get_max_health());
        }
    }
}


CombatWindow::CombatWindow(int x, int y, int width, int height, ViewResources *resources, CombatView *view, UnitRenderer *renderer):
        UiWindow(x, y, width, height, WindowIsVisible) {
    Point centre(0, 0);
    int centre_x = width/2;
    int centre_y = height/2;

    for (int dir = 0; dir <= 6; dir++) {
        Point relative_point = get_neighbour(centre, dir);
        x = centre_x + relative_point.x * (battle_stack_width + battle_stack_padding) - battle_stack_width/2;
        y = centre_y + relative_point.y * (battle_stack_height + battle_stack_padding) - battle_stack_height/2 + (relative_point.x ? (battle_stack_height + battle_stack_padding) / 2 : 0);

        stack_windows[dir] = new CombatStackWindow(x, y, battle_stack_width, battle_stack_height, resources, view, renderer, dir);
        add_child(stack_windows[dir]);
    }
}
