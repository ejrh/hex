#ifndef COMBAT_VIEW_H
#define COMBAT_VIEW_H

#include "hexview/view/view.h"
#include "hexview/view/view_resources.h"

class Participant;
class UnitStack;
class Battle;

class ParticipantView: public UnitView {
public:
    ParticipantView(Participant *participant);

public:
    Participant *participant;
    int x, y;
};

class CombatStackView {
public:
    CombatStackView();

    void add_participant(ParticipantView& pv);

public:
    unsigned int phase;
    int x, y;
    int participants[12];
};

class CombatView {
public:
    CombatView(Battle *battle, int width, int height, ViewResources *resources);

    void update();
    void step();

    static const int participant_width = 60;
    static const int participant_height = 50;
    static const int battle_stack_width = participant_width * 4;
    static const int battle_stack_height = participant_height * 3;
    static const int battle_stack_padding = 8;

public:
    Battle *battle;
    int width, height;
    ViewResources *resources;
    UnitPainter unit_painter;
    CombatStackView combat_stack_views[7];
    std::vector<ParticipantView> participant_views;
    unsigned int last_update;
    unsigned int phase;
    unsigned int current_move;
    unsigned int current_step;
    unsigned int phase_end;
};

#endif
