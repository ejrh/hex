#ifndef STRUCTURE_PAINTER_H
#define STRUCTURE_PAINTER_H

class Game;
class GameView;
class Resources;
class StructureView;

class StructurePainter {
public:
    StructurePainter(Game *game, GameView *view, Resources *resources):
            game(game), view(view), resources(resources) { }

    void repaint(StructureView& structure_view);

private:
    Game *game;
    GameView *view;
    Resources *resources;
};

#endif
