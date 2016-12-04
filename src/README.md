Hex Implementation Notes
========================

The markdown (`*.md`) files found in this directory pertain to the implementation of Hex.

  - [COMBAT.md](COMBAT.md) - combat mechanics.
  - [COMMANDS.md](COMMANDS.md) - command messages, which are sent by clients to a server to
    indicate player actions.
  - [PROTOCOL.md](PROTOCOL.md) - the network protocol.
  - [SERIALISATION.md](SERIALISATION.md) - the serialisation format used for networking messages
    and resource and game definition files.
  - [UI.md](UI.md) - the UI system.

Components of Hex
-----------------

Components:
  * hexutil - Library of basic utilities, not game-specific
  * hexav - Library of audio, visual and general UI functionality
  * hexgame - Library containing Hex game implementation (game objects and mechanics)
  * hexview - Library containing Hex view implementation (game windows, resources)
  * hexmain - Executable game
  * tests - Various tests

The approximate order of dependencies between modules is depicted by this diagram:

                      +------------------+
                      |     hexutil      |
    +------------+    |                  |    +--------------------------+
    |  hexgame   |    |    +--------+    |    |          hexav           |
    |            |    |    | basics |    |    |                          |
    |  +------+  |    |    +---+----+    |    |            +----------+  |
    |  | game |  |    |        |         |    |            | graphics |  |
    |  +--+---+  <----+  +-----v-----+   +---->            +----+-----+  |
    |     |      |    |  | messaging |   |    |                 |        |
    |   +-v--+   |    |  +-----+-----+   |    |  +-------+    +-v--+     |
    |   | ai |   |    |        |         |    |  | audio |    | ui |     |
    |   +----+   |    |  +-----v------+  |    |  +-------+    +----+     |
    |            |    |  | networking |  |    |                          |
    +-----+------+    |  +------------+  |    +------------+-------------+
          |           |                  |                 |
          |           +---------+--------+                 |
          |                     |                          |
          |      +--------------+--------------+           |
          |      |          hexview            |           |
          +------>                             <-----------+
                 |  +-----v-----+    +v-----+  |
                 |  | resources |    | chat |  |
                 |  +-----+-----+    +---+--+  |
                 |        |              |     |
                 |        | +------+     |     |
                 |        +-> view <-----+     |
                 |          +------+           |
                 |                             |
                 +-------------+---------------+
                               |
                          +----+----+
                          | hexmain |
                          +---------+

Object relationships
--------------------

Key:

  - `[]` indicates a collection or array (`[][]` a two-dimensional array)
  - `*` indicates a reference to an object owned elsewhere

Objects:

    Game
        TileType[]
        UnitType[]
        StructureType[]
        Faction[]
        Level
            Tile[][]
                TileType *
                Structure *
        UnitStack[]
            Unit[]
                UnitType *

    GameView
        Game *
        Player *
        FactionView[]
            FactionViewDef *
        LevelView
            TileView[][]
                TileViewDef *
                StructureView *
                    StructureViewDef *
        UnitStackView[]
            UnitViewDef *

    Resources
        Image[]
        Sound[]
        ImageSeries[]
        TileViewDef[]
        UnitViewDef[]
        StructureViewDef[]
        FactionViewDef[]
        Song[]
