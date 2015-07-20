set(srcs
    noise.cpp
    noise.h
    pathfinding.cpp
    pathfinding.h
    basics/error.cpp
    basics/error.h
    basics/hexgrid.cpp
    basics/hexgrid.h
    basics/point.h
    basics/vector2.h
    game/game.cpp
    game/game.h
    game/game_arbiter.cpp
    game/game_arbiter.h
    game/game_messages.cpp
    game/game_messages.h
    game/game_serialisation.h
    game/game_updater.cpp
    game/game_updater.h
    game/level.cpp
    game/message_types.h
    graphics/graphics.cpp
    graphics/graphics.h
    graphics/ilb.cpp
    graphics/load_image.cpp
    messaging/dispatcher.h
    messaging/message.cpp
    messaging/receiver.cpp
    messaging/receiver.h
    messaging/serialiser.h
    messaging/updater.cpp
    messaging/updater.h
    networking/client.cpp
    networking/connection.cpp
    networking/networking.h
    networking/server.cpp
    resources/image_loader.cpp
    resources/image_ref.h
    resources/resource_loader.cpp
    resources/resources.cpp
    resources/resources.h
    resources/view_def.h
    resources/view_def_serialisation.h
    view/level_renderer.cpp
    view/level_renderer.h
    view/level_view.cpp
    view/view.h
    view/view_updater.cpp
    view/view_updater.h
)

source_group(Sources REGULAR_EXPRESSION cpp|h)
source_group(Sources\\basics REGULAR_EXPRESSION basics)
source_group(Sources\\game REGULAR_EXPRESSION game)
source_group(Sources\\graphics REGULAR_EXPRESSION graphics)
source_group(Sources\\messaging REGULAR_EXPRESSION messaging)
source_group(Sources\\networking REGULAR_EXPRESSION networking)
source_group(Sources\\resources REGULAR_EXPRESSION resources)
source_group(Sources\\view REGULAR_EXPRESSION view)
add_library(hex ${srcs})
target_compile_options(hex PUBLIC ${gen_opts})
