#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/basics/vector2.h"

#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"

#include "hexview/view/unit_renderer.h"
#include "hexview/view/view.h"


namespace hex {

UnitRenderer::UnitRenderer(Graphics *graphics, Resources *resources):
        generate_placeholders(true),
        graphics(graphics), resources(resources) {
}

UnitRenderer::~UnitRenderer() {
}

void UnitRenderer::draw_unit(int x, int y, UnitView& unit_view) {
    graphics->fill_rectangle(0,0,0, x-1,y-1, 2,2);
    unit_view.paint.render(x, y, unit_view.phase, graphics);
}

void UnitRenderer::draw_unit_centered(int x, int y, UnitView& unit_view) {
    SDL_Rect bounds = unit_view.paint.get_bounds();
    int px = x - bounds.x - bounds.w / 2;
    int py = y - bounds.y - bounds.h / 2;
    unit_view.paint.render(px, py, unit_view.phase, graphics);
}

void UnitRenderer::draw_health_bar(int x, int y, int w, int h, int health, int max_health) {
    static const int R[] = { 255, 228, 0 };
    static const int G[] = { 0, 228, 255 };
    static const int B[] = { 0, 0, 0 };

    graphics->fill_rectangle(0, 0, 0, x, y, w, h);
    if (health == 0)
        return;
    // width
    float f = health / static_cast<float>(max_health);
    int w2 = static_cast<int>(f * (w - 2));
    // colour
    float f2 = (health - 1) / static_cast<float>(max_health - 1);
    float f2b = (f2 < 0.5f) ? (f2 * 2) : ((f2 - 0.5f) * 2);
    float f2a = 1.0f - f2b;
    int r = static_cast<int>((f2 < 0.5) ? (R[0]*f2a + R[1]*f2b) : (R[1]*f2a + R[2]*f2b));
    int g = static_cast<int>((f2 < 0.5) ? (G[0]*f2a + G[1]*f2b) : (G[1]*f2a + G[2]*f2b));
    int b = static_cast<int>((f2 < 0.5) ? (B[0]*f2a + B[1]*f2b) : (B[1]*f2a + B[2]*f2b));
    graphics->fill_rectangle(r, g, b, x + 1, y + 1, w2, h - 2);
}

};
