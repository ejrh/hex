Hex Implementation Notes
========================

Components:
  * hexlib - Library containing actual Hex implementation
  * hexmain - Executable game


Modules
-------

Approximate order of dependencies between modules:

             basics
    graphics        messaging
                      game
           networking
           resources
              view


Object relationships
--------------------

    Game
        Level
            Tile[][]
                UnitStack *
        UnitStack[]
            Unit[]
                UnitType *
        UnitType[]

    GameView
        LevelView
            TileView[][]
                TileViewDef *
        UnitStackView[]
            UnitViewDef *

    Resources
        Image[]
        ImageSeries[]
        TileViewDef[]
        UnitViewDef[]
