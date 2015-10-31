#ifndef LEVEL_RENDERER_H
#define LEVEL_RENDERER_H

#include "hex/basics/point.h"
#include "hex/resources/image_ref.h"


class Graphics;
class Resources;
class Level;
class GameView;
class LevelView;
class UnitStackView;
class Unit;
class UnitViewDef;

class LevelRenderer {
public:
    typedef void (LevelRenderer::*RenderMethod)(int x, int y, Point tile_pos);

    LevelRenderer(Graphics *graphics, Resources *resources, Level *level, GameView *view);
    ~LevelRenderer();

protected:
    void clear(int x, int y, int width, int height);
    virtual void render_tile(int x, int y, Point tile_pos);
    virtual void render_tile_transitions(int x, int y, Point tile_pos);
    virtual void render_features(int x, int y, Point tile_pos);
    virtual void render_objects(int x, int y, Point tile_pos);
    virtual void draw_unit_stack(int x, int y, UnitStackView& stack_view);
    virtual void draw_unit(int x, int y, Unit &unit, UnitViewDef& view_def, int highlight = 0);
    virtual void render_path_arrow(int x, int y, Point tile_pos);

    Image *get_image_or_placeholder(ImageSeries& image_series, int pos, const std::string name);

public:
    bool show_hexagons;

protected:
    Graphics *graphics;
    Resources *resources;
    Level *level;
    GameView *view;

    ImageSeries cursor_images;
    ImageSeries arrow_images;

    friend class LevelWindow;
    friend class StackWindow;
};

#endif
