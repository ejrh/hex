#ifndef UNIT_RENDERER_H
#define UNIT_RENDERER_H

#include "hexutil/basics/point.h"

#include "hexav/resources/image_ref.h"


namespace hex {

class Graphics;
class Resources;
class UnitView;

class UnitRenderer {
public:
    UnitRenderer(Graphics *graphics, Resources *resources);
    ~UnitRenderer();
    virtual void draw_unit(int x, int y, UnitView& unit_view);
    virtual void draw_unit_centered(int x, int y, UnitView& unit_view);
    void draw_health_bar(int x, int y, int w, int h, int health, int max_health);

public:
    bool generate_placeholders;

protected:
    Graphics *graphics;
    Resources *resources;

    friend class LevelRenderer;
};

};

#endif
