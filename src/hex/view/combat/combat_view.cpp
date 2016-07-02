#include "common.h"

#include "hex/basics/hexgrid.h"
#include "hex/game/game.h"
#include "hex/game/combat/combat.h"
#include "hex/game/combat/combat_model.h"
#include "hex/game/combat/move_types.h"
#include "hex/view/combat/combat_view.h"


ParticipantView::ParticipantView(Participant *participant):
        participant(participant) {
}

BattleStackView::BattleStackView() {
    for (int i = 0; i < 12; i++) {
        participants[i] = -1;
    }
}

void BattleStackView::add_participant(ParticipantView& pv) {
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
    pv.x = x + (num % 4) * BattleView::participant_width + BattleView::participant_width / 2;
    pv.y = y + (num / 4) * BattleView::participant_height + BattleView::participant_height;
}

BattleView::BattleView(Battle *battle, int width, int height, Resources *resources):
        battle(battle), width(width), height(height), resources(resources), last_update(0),
        phase(0), current_move(0), current_step(0), phase_end(1000) {
    Point centre(0, 0);
    int centre_x = width/2;
    int centre_y = height/2;
    for (int dir = 0; dir <= 6; dir++) {
        BattleStackView& bsv = battle_stack_views[dir];
        Point relative_point;
        get_neighbour(centre, dir, &relative_point);
        bsv.x = centre_x + relative_point.x * (battle_stack_width + battle_stack_padding) - battle_stack_width/2;
        bsv.y = centre_y + relative_point.y * (battle_stack_height + battle_stack_padding) - battle_stack_height/2 + (relative_point.x ? (battle_stack_height + battle_stack_padding) / 2 : 0);

        for (int i = 0; i < 12; i++)
            bsv.participants[i] = -1;
    }

    for (std::vector<Participant>::iterator iter = battle->participants.begin(); iter != battle->participants.end(); iter++) {
        Participant& p = *iter;
        ParticipantView pv(&p);
        pv.view_def = resources->get_unit_view_def(p.unit->type->name);
        pv.facing = 2;
        pv.posture = Holding;

        BattleStackView& bsv = battle_stack_views[p.stack_num];
        bsv.add_participant(pv);

        participant_views.push_back(pv);
    }
}

void BattleView::update() {
    unsigned int ticks = SDL_GetTicks();
    if (ticks < last_update + 25)
        return;
    unsigned int update_ms = ticks - last_update;
    last_update = ticks;

    for (std::vector<ParticipantView>::iterator iter = participant_views.begin(); iter != participant_views.end(); iter++) {
        iter->update(update_ms);
    }

    phase += update_ms;
    if (phase >= phase_end && current_move < battle->moves.size()) {
        step();
        phase = 0;
    }
}

void BattleView::step() {
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
            phase_end = 250;
        } break;

        case 1: {
            pv.selected = true;
            pv.phase = 0;
            pv.posture = Attacking;
            phase_end = pv.get_animation_def().duration();
        } break;

        case 2: {
            pv.posture = Holding;
            bool hurt = move_type->direction == Detrimental && move.effect != 0;
            tv.phase = 0;
            tv.posture = hurt ? Recoiling : Holding;
            phase_end = tv.get_animation_def().duration();
        } break;

        case 3: {
            battle->replay_move(move);
            pv.selected = false;
            tv.posture = tv.participant->is_alive() ? Holding : Dying;
            if (tv.posture == Dying) {
                tv.facing = rand() % 6;
            }
            phase_end = tv.get_animation_def().duration();
        }
    }
}
