#ifndef LEVEL_RENDERER_H
#define LEVEL_RENDERER_H

class Graphics;
class Resources;
class Level;

class LevelRenderer {
public:
    typedef void (LevelRenderer::*RenderMethod)(int x, int y, Point tile_pos);

    LevelRenderer(Graphics *graphics, Resources *resources, Level *level, GameView *view, LevelView *level_view);
    ~LevelRenderer();

protected:
    void clear(int x, int y, int width, int height);
    virtual void render_tile(int x, int y, Point tile_pos);
    virtual void render_unit_stack(int x, int y, Point tile_pos);
    virtual void draw_unit_stack(int x, int y, UnitStackView &stack_view);
    virtual void draw_unit(int x, int y, Unit &unit, UnitViewDef *view_def);
    virtual void render_path_arrow(int x, int y, Point tile_pos);

protected:
    Graphics *graphics;
    Resources *resources;
    Level *level;
    GameView *view;
    LevelView *level_view;

    ImageSeries cursor_images;
    ImageSeries arrow_images;

    friend class LevelWindow;
    friend class StackWindow;
};

#endif
