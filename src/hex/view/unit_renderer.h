#ifndef UNIT_RENDERER_H
#define UNIT_RENDERER_H

#include "hex/basics/point.h"
#include "hex/resources/image_ref.h"


class Graphics;
class Resources;
class UnitView;

class UnitRenderer {
public:
    UnitRenderer(Graphics *graphics, Resources *resources);
    ~UnitRenderer();
    virtual void draw_unit(int x, int y, UnitView& unit_view);
    virtual void draw_unit_centered(int x, int y, UnitView& unit_view);

public:
    bool generate_placeholders;

protected:
    const ImageSeries& get_image_series(const UnitView& unit_view) const;
    Image *get_image_or_placeholder(ImageSeries& image_series, int pos, const std::string name);

protected:
    Graphics *graphics;
    Resources *resources;

    friend class LevelRenderer;
};

#endif
