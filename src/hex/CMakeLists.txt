set(srcs
    basics/error.cpp
    basics/hexgrid.cpp
    game/game.cpp
    game/game_messages.cpp
    game/game_updater.cpp
    game/game_arbiter.cpp
    game/level.cpp
    pathfinding.cpp
    graphics/graphics.cpp
    graphics/ilb.cpp
    graphics/load_image.cpp
    noise.cpp
    resources/resources.cpp
    resources/resource_loader.cpp
    resources/image_loader.cpp
    view/view_updater.cpp
    view/level_view.cpp
    view/level_renderer.cpp
    messaging/message.cpp
    messaging/receiver.cpp
    messaging/updater.cpp
)

add_library(hex ${srcs})
target_compile_options(hex PUBLIC "-Wall" "-g")
