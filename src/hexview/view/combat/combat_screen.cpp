#include "common.h"

#include "hexav/audio/audio.h"
#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"
#include "hexav/ui/ui.h"

#include "hexgame/game/game.h"

#include "hexview/view/level_renderer.h"
#include "hexview/view/combat/combat_screen.h"
#include "hexview/view/combat/combat_view.h"
#include "hexview/view/combat/combat_window.h"
#include "hexview/view/view.h"

namespace combat_screen {

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

CombatScreen::CombatScreen(ViewResources *resources, Graphics *graphics, Audio *audio, GameView *game_view, UnitRenderer *renderer):
        resources(resources), graphics(graphics), audio(audio), game_view(game_view), renderer(renderer) { }

void CombatScreen::show_battle(Battle *battle) {
    CombatView combat_view(battle, graphics->width, graphics->height, resources);

    UiLoop loop(graphics, 25);
    combat_screen::BackgroundWindow *bg_window = new combat_screen::BackgroundWindow(&loop);
    loop.set_root_window(bg_window);
    CombatWindow *combat_window = new CombatWindow(100, 100, graphics->width - 200, graphics->height - 200, resources, graphics, &combat_view, renderer);
    bg_window->add_child(combat_window);
    combat_screen::TopWindow *top_window = new combat_screen::TopWindow(graphics, audio);
    bg_window->add_child(top_window);
    loop.run();
}
