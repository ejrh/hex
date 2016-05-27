#ifndef BATTLE_VIEWER_H
#define BATTLE_VIEWER_H


class Battle;
class Resources;
class Graphics;
class Audio;
class GameView;
class LevelRenderer;

class BattleViewer {
public:
    BattleViewer(Resources *resources, Graphics *graphics, Audio *audio, GameView *view, LevelRenderer *renderer);
    void show_battle(Battle *battle);

private:
    Resources *resources;
    Graphics *graphics;
    Audio *audio;
    GameView *game_view;
    LevelRenderer *renderer;
};


#endif
