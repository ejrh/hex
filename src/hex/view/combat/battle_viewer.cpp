#include "common.h"

#include "hex/audio/audio.h"
#include "hex/game/game.h"
#include "hex/graphics/font.h"
#include "hex/graphics/graphics.h"
#include "hex/ui/ui.h"
#include "hex/view/level_renderer.h"
#include "hex/view/combat/combat_view.h"
#include "hex/view/combat/battle_viewer.h"
#include "hex/view/combat/battle_window.h"
#include "hex/view/view.h"

namespace battle_viewer {

class BackgroundWindow: public UiWindow {
public:
    BackgroundWindow(UiLoop *loop): UiWindow(0, 0, 0, 0), loop(loop) { }

    bool receive_event(SDL_Event *evt) {
        if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE) {
            loop->running = false;
            return true;
        }

        return false;
    }

    bool contains(int px, int py) {
        return true;
    }

private:
    UiLoop *loop;
};


class TopWindow: public UiWindow {
public:
    TopWindow(Graphics *graphics, Audio *audio): UiWindow(0, 0, 0, 0), graphics(graphics), audio(audio) { }

    void draw() {
        graphics->update();
        audio->update();
    }

private:
    Graphics *graphics;
    Audio *audio;
};

}

BattleViewer::BattleViewer(Resources *resources, Graphics *graphics, Audio *audio, GameView *game_view, UnitRenderer *renderer):
        resources(resources), graphics(graphics), audio(audio), game_view(game_view), renderer(renderer) { }

void BattleViewer::show_battle(Battle *battle) {
    BattleView battle_view(battle, graphics->width, graphics->height, resources);

    UiLoop loop(25);
    battle_viewer::BackgroundWindow bg_window(&loop);
    loop.add_window(&bg_window);
    BattleWindow battle_window(100, 100, graphics->width - 200, graphics->height - 200, resources, graphics, &battle_view, renderer);
    loop.add_window(&battle_window);
    battle_viewer::TopWindow top_window(graphics, audio);
    loop.add_window(&top_window);
    loop.run();
}
