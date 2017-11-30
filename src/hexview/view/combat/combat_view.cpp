#include "common.h"

#include "hexutil/basics/hexgrid.h"

#include "hexgame/game/game.h"
#include "hexgame/game/combat/combat.h"
#include "hexgame/game/combat/combat_model.h"
#include "hexgame/game/combat/move_types.h"

#include "hexview/view/combat/combat_view.h"
#include "hexview/view/unit_painter.h"


ParticipantView::ParticipantView(Participant *participant):
        participant(participant) {
}

CombatStackView::CombatStackView() {
    for (int i = 0; i < 12; i++) {
        participants[i] = -1;
    }
}

void CombatStackView::add_participant(ParticipantView& pv) {
    int num = -1;
    for (int i = 0; i < 12; i++) {
        if (participants[i] == -1) {
            num = i;
            break;
        }
    }

    if (num == -1) {
        BOOST_LOG_TRIVIAL(warning) << "Can't move participant to stack";
        return;
    }

    participants[num] = pv.participant->id;
    pv.x = x + (num % 4) * CombatView::participant_width + CombatView::participant_width / 2;
    pv.y = y + (num / 4) * CombatView::participant_height + CombatView::participant_height / 2;
}

CombatView::CombatView(Battle *battle, int width, int height, ViewResources *resources):
        battle(battle), width(width), height(height), resources(resources), unit_painter(battle->game, NULL, resources),
        last_update(0),
        phase(0), current_move(0), current_step(0), phase_end(1000) {
    Point centre(0, 0);
    int centre_x = width/2;
    int centre_y = height/2;
    for (int dir = 0; dir <= 6; dir++) {
        CombatStackView& bsv = combat_stack_views[dir];
        Point relative_point;
        get_neighbour(centre, dir, &relative_point);
        bsv.x = centre_x + relative_point.x * (battle_stack_width + battle_stack_padding) - battle_stack_width/2;
        bsv.y = centre_y + relative_point.y * (battle_stack_height + battle_stack_padding) - battle_stack_height/2 + (relative_point.x ? (battle_stack_height + battle_stack_padding) / 2 : 0);

        for (int i = 0; i < 12; i++)
            bsv.participants[i] = -1;
    }

    for (auto iter = battle->participants.begin(); iter != battle->participants.end(); iter++) {
        Participant& p = *iter;
        ParticipantView pv(&p);
        pv.view_def = resources->get_unit_view_def(p.unit->type->name);
        pv.facing = 2;
        pv.posture = Holding;
        unit_painter.repaint(pv, *p.unit);

        CombatStackView& bsv = combat_stack_views[p.stack_num];
        bsv.add_participant(pv);

        participant_views.push_back(pv);
    }
}

void CombatView::update() {
    unsigned int ticks = SDL_GetTicks();
    if (ticks < last_update + 25)
        return;
    unsigned int update_ms = ticks - last_update;
    last_update = ticks;

    for (auto iter = participant_views.begin(); iter != participant_views.end(); iter++) {
        iter->update(update_ms);
    }

    phase += update_ms;
    if (phase >= phase_end && current_move < battle->moves.size()) {
        step();
        phase = 0;
    }
}

void CombatView::step() {
    current_step++;
    if (current_step > 4) {
        current_step = 0;
        current_move++;
    }

    if (current_move >= battle->moves.size())
        return;

    Move& move = battle->moves[current_move];
    const MoveType *move_type = battle->combat_model->get_move_type(move);
    std::cerr << "Move " << current_move << ": " << move << std::endl;

    ParticipantView& pv = participant_views[move.participant_id];
    ParticipantView& tv = participant_views[move.target_id];

    switch (current_step) {
        case 0: {
            pv.selected = true;
            phase_end = 250;
        } break;

        case 1: {
            tv.targetted = true;
            pv.phase = 0;
            pv.posture = Attacking;
        } break;

        case 2: {
            pv.posture = Holding;
            bool hurt = move_type->direction == Detrimental && move.effect != 0;
            tv.phase = 0;
            tv.posture = hurt ? Recoiling : Holding;
        } break;

        case 3: {
            battle->replay_move(move);
            pv.selected = false;
            tv.targetted = false;
            tv.posture = tv.participant->is_alive() ? Holding : Dying;
            phase_end = 100;
        }
    }

    unit_painter.repaint(pv, *pv.participant->unit);
    unit_painter.repaint(tv, *tv.participant->unit);

    switch (current_step) {
        case 1: {
            phase_end = pv.paint.get_duration();
        } break;

        case 2: {
            phase_end = tv.paint.get_duration();
        } break;
    }
}
