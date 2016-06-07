#ifndef COMBAT_VIEW_H
#define COMBAT_VIEW_H

class Participant;
class UnitStack;
class Battle;

class ParticipantView {
public:
    ParticipantView(Participant *participant);

public:
    Participant *participant;
    int x, y;
};

class BattleStackView {
public:
    BattleStackView() { }

    void add_participant(ParticipantView& pv);

public:
    unsigned int phase;
    int x, y;
    int participants[12];
};

class BattleView {
public:
    BattleView(Battle *battle, int width, int height);

    static const int participant_width = 60;
    static const int participant_height = 50;
    static const int battle_stack_width = participant_width * 4;
    static const int battle_stack_height = participant_height * 3;
    static const int battle_stack_padding = 8;

public:
    Battle *battle;
    int width, height;
    BattleStackView battle_stack_views[7];
    std::vector<ParticipantView> participant_views;
};

#endif