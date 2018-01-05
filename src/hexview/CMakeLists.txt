set(srcs
        chat/chat.h
        chat/chat_updater.cpp
        chat/chat_window.cpp
        editor/brush.cpp
        editor/brush.h
        editor/editor.cpp
        editor/editor.h
        editor/palette.cpp
        editor/palette.h
        view/combat/combat_screen.cpp
        view/combat/combat_screen.h
        view/combat/combat_view.cpp
        view/combat/combat_view.h
        view/combat/combat_window.cpp
        view/combat/combat_window.h
        view/audio_renderer.cpp
        view/audio_renderer.h
        view/game_view.cpp
        view/ghost.cpp
        view/ghost.h
        view/level_renderer.cpp
        view/level_renderer.h
        view/level_view.cpp
        view/level_window.cpp
        view/level_window.h
        view/map_window.cpp
        view/map_window.h
        view/message_types.h
        view/message_window.cpp
        view/message_window.h
        view/player.cpp
        view/player.h
        view/pre_updater.cpp
        view/pre_updater.h
        view/stack_window.cpp
        view/stack_window.h
        view/status_window.cpp
        view/status_window.h
        view/structure_painter.cpp
        view/structure_painter.h
        view/tile_painter.cpp
        view/tile_painter.h
        view/transition_paint.cpp
        view/transition_paint.h
        view/unit_info_window.cpp
        view/unit_info_window.h
        view/unit_painter.cpp
        view/unit_painter.h
        view/unit_renderer.cpp
        view/unit_renderer.h
        view/unit_stack_view.cpp
        view/view.h
        view/view_resource_loader.cpp
        view/view_resource_loader.h
        view/view_resources.cpp
        view/view_resources.h
        view/view_resource_messages.cpp
        view/view_resource_messages.h
        view/view_updater.cpp
        view/view_updater.h
)

source_group(Sources REGULAR_EXPRESSION cpp|h)
source_group(Sources\\chat REGULAR_EXPRESSION /chat/)
source_group(Sources\\resources REGULAR_EXPRESSION /resources/)
source_group(Sources\\view REGULAR_EXPRESSION /view/)
source_group(Sources\\view\\combat REGULAR_EXPRESSION /view/combat/)
add_library(hexview ${srcs})
target_compile_options(hexview PUBLIC ${gen_opts})
if (COMMAND cotire)
    cotire(hexview)
endif()
