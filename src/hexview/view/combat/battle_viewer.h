#ifndef BATTLE_VIEWER_H
#define BATTLE_VIEWER_H


class Battle;
class Resources;
class Graphics;
class Audio;
class GameView;
class UnitRenderer;

class BattleViewer {
public:
    BattleViewer(Resources *resources, Graphics *graphics, Audio *audio, GameView *view, UnitRenderer *renderer);
    void show_battle(Battle *battle);

private:
    Resources *resources;
    Graphics *graphics;
    Audio *audio;
    GameView *game_view;
    UnitRenderer *renderer;
};


#endif
