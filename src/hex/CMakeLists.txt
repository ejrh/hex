set(srcs
        noise.cpp
        noise.h
        ai/ai.cpp
        ai/ai.h
        ai/ai_updater.cpp
        ai/ai_updater.h
        audio/audio.cpp
        audio/audio.h
        basics/error.cpp
        basics/error.h
        basics/hexgrid.cpp
        basics/hexgrid.h
        basics/point.h
        basics/vector2.h
        chat/chat.h
        chat/chat_updater.cpp
        chat/chat_window.cpp
        game/game.cpp
        game/game.h
        game/game_arbiter.cpp
        game/game_arbiter.h
        game/game_messages.cpp
        game/game_messages.h
        game/game_serialisation.h
        game/game_updater.cpp
        game/game_updater.h
        game/game_writer.cpp
        game/game_writer.h
        game/level.cpp
        game/message_types.h
        game/movement.cpp
        game/movement.h
        game/pathfinding.cpp
        game/pathfinding.h
        game/trait_types.h
        game/traits.cpp
        game/traits.h
        game/unit_stack.cpp
        game/visibility_map.cpp
        game/visibility_map.h
        graphics/font.cpp
        graphics/font.h
        graphics/graphics.cpp
        graphics/graphics.h
        graphics/hss.cpp
        graphics/ilb.cpp
        graphics/ilb.h
        graphics/load_image.cpp
        messaging/event_pusher.cpp
        messaging/event_pusher.h
        messaging/message.cpp
        messaging/message.h
        messaging/receiver.cpp
        messaging/receiver.h
        messaging/serialiser.h
        messaging/updater.cpp
        messaging/updater.h
        messaging/writer.h
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
        ui/loop.cpp
        ui/ui.h
        ui/ui_window.cpp
        view/game_view.cpp
        view/level_renderer.cpp
        view/level_renderer.h
        view/level_view.cpp
        view/level_window.cpp
        view/level_window.h
        view/map_window.cpp
        view/map_window.h
        view/player.cpp
        view/player.h
        view/stack_window.cpp
        view/stack_window.h
        view/status_window.cpp
        view/status_window.h
        view/tile_painter.cpp
        view/tile_painter.h
        view/view.h
        view/view_updater.cpp
        view/view_updater.h
)

source_group(Sources REGULAR_EXPRESSION cpp|h)
source_group(Sources\\ai REGULAR_EXPRESSION /ai/)
source_group(Sources\\audio REGULAR_EXPRESSION /audio/)
source_group(Sources\\basics REGULAR_EXPRESSION /basics/)
source_group(Sources\\chat REGULAR_EXPRESSION /chat/)
source_group(Sources\\game REGULAR_EXPRESSION /game/)
source_group(Sources\\graphics REGULAR_EXPRESSION /graphics/)
source_group(Sources\\messaging REGULAR_EXPRESSION /messaging/)
source_group(Sources\\networking REGULAR_EXPRESSION /networking/)
source_group(Sources\\resources REGULAR_EXPRESSION /resources/)
source_group(Sources\\ui REGULAR_EXPRESSION /ui/)
source_group(Sources\\view REGULAR_EXPRESSION /view/)
add_library(hex ${srcs})
target_compile_options(hex PUBLIC ${gen_opts})

if (COMMAND cotire)
    cotire(hex)
endif()
