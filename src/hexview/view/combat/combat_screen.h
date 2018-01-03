#ifndef COMBAT_SCREEN_H
#define COMBAT_SCREEN_H

namespace hex {

class Battle;
class ViewResources;
class Graphics;
class Audio;
class GameView;
class UnitRenderer;

class CombatScreen {
public:
    CombatScreen(ViewResources *resources, Graphics *graphics, Audio *audio, GameView *view, UnitRenderer *renderer);
    void show_battle(Battle *battle);

private:
    ViewResources *resources;
    Graphics *graphics;
    Audio *audio;
    GameView *game_view;
    UnitRenderer *renderer;
};

};

#endif
