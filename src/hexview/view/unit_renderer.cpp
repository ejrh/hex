#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/basics/vector2.h"

#include "hexav/graphics/font.h"
#include "hexav/graphics/graphics.h"

#include "hexview/view/unit_renderer.h"
#include "hexview/view/view.h"


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
    // TODO - determine centre of painted area (using clip data)
    // and draw appropriately
}

void UnitRenderer::draw_health_bar(int x, int y, int w, int h, int health, int max_health) {
    static const int R[] = { 255, 228, 0 };
    static const int G[] = { 0, 228, 255 };
    static const int B[] = { 0, 0, 0 };

    graphics->fill_rectangle(0, 0, 0, x, y, w, h);
    if (health == 0)
        return;
    // width
    float f = health / (float) max_health;
    int w2 = f * (w - 2);
    // colour
    float f2 = (health - 1) / (float) (max_health - 1);
    float f2b = (f2 < 0.5) ? (f2 * 2) : ((f2 - 0.5) * 2);
    float f2a = 1.0 - f2b;
    int r = (f2 < 0.5) ? (R[0]*f2a + R[1]*f2b) : (R[1]*f2a + R[2]*f2b);
    int g = (f2 < 0.5) ? (G[0]*f2a + G[1]*f2b) : (G[1]*f2a + G[2]*f2b);
    int b = (f2 < 0.5) ? (B[0]*f2a + B[1]*f2b) : (B[1]*f2a + B[2]*f2b);
    graphics->fill_rectangle(r, g, b, x + 1, y + 1, w2, h - 2);
}
