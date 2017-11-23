#include "common.h"

#include "hexav/audio/audio.h"
#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"
#include "hexav/ui/ui.h"

#include "hexgame/game/game.h"

#include "hexview/view/level_renderer.h"
#include "hexview/view/combat/combat_view.h"
#include "hexview/view/combat/battle_viewer.h"
#include "hexview/view/combat/battle_window.h"
#include "hexview/view/view.h"

namespace battle_viewer {

class BackgroundWindow: public UiWindow {
public:
    BackgroundWindow(UiLoop *loop):
            UiWindow(0, 0, 0, 0, WindowIsVisible|WindowIsActive|WindowWantsKeyboardEvents),
            loop(loop) { }

    bool receive_keyboard_event(SDL_Event *evt) {
        if (evt->type == SDL_KEYDOWN && evt->key.keysym.sym == SDLK_ESCAPE) {
            loop->running = false;
            return true;
        }

        return false;
    }

private:
    UiLoop *loop;
};


class TopWindow: public UiWindow {
public:
    TopWindow(Graphics *graphics, Audio *audio):
            UiWindow(0, 0, 0, 0, WindowIsVisible),
            graphics(graphics), audio(audio) { }

    void draw(const UiContext& context) {
        graphics->update();
        audio->update();
    }

private:
    Graphics *graphics;
    Audio *audio;
};

}

BattleViewer::BattleViewer(ViewResources *resources, Graphics *graphics, Audio *audio, GameView *game_view, UnitRenderer *renderer):
        resources(resources), graphics(graphics), audio(audio), game_view(game_view), renderer(renderer) { }

void BattleViewer::show_battle(Battle *battle) {
    BattleView battle_view(battle, graphics->width, graphics->height, resources);

    UiLoop loop(graphics, 25);
    battle_viewer::BackgroundWindow *bg_window = new battle_viewer::BackgroundWindow(&loop);
    loop.set_root_window(bg_window);
    BattleWindow *battle_window = new BattleWindow(100, 100, graphics->width - 200, graphics->height - 200, resources, graphics, &battle_view, renderer);
    bg_window->add_child(battle_window);
    battle_viewer::TopWindow *top_window = new battle_viewer::TopWindow(graphics, audio);
    bg_window->add_child(top_window);
    loop.run();
}
