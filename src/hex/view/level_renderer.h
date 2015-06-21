#ifndef LEVEL_RENDERER_H
#define LEVEL_RENDERER_H

class LevelRenderer {
public:
    typedef void (LevelRenderer::*RenderMethod)(int x, int y, Point tile_pos);

    LevelRenderer(Graphics *graphics, Resources *resources, boost::shared_ptr<Level> level, LevelView *level_view);
    ~LevelRenderer();
    void draw();
    void draw_level(RenderMethod render);
    /*void draw_floor();
    void draw_units();
    void draw_ceiling();*/

protected:
    virtual void render_tile(int x, int y, Point tile_pos);
    virtual void render_unit_stack(int x, int y, Point tile_pos);
    virtual void draw_unit_stack(int x, int y, UnitStackView &stack_view);
    virtual void draw_moving_unit();
    virtual void render_path_arrow(int x, int y, Point tile_pos);

protected:
    Graphics *graphics;
    Resources *resources;
    boost::shared_ptr<Level> level;
    LevelView *level_view;

    ImageSeries cursor_images;
    ImageSeries arrow_images;
};

#endif
