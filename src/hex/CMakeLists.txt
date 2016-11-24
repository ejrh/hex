set(srcs
        ai/ai.cpp
        ai/ai.h
        ai/ai_updater.cpp
        ai/ai_updater.h
        audio/audio.cpp
        audio/audio.h
        chat/chat.h
        chat/chat_updater.cpp
        chat/chat_window.cpp
        game/end_of_turn.cpp
        game/end_of_turn.h
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
        game/generation/generator.cpp
        game/generation/generator.h
        game/generation/level_generator.cpp
        game/generation/level_generator.h
        game/level.cpp
        game/message_types.h
        game/combat/battle.cpp
        game/combat/combat.h
        game/combat/combat_model.cpp
        game/combat/combat_model.h
        game/combat/move.h
        game/combat/move_types.h
        game/combat/move_types.cpp
        game/combat/participant.cpp
        game/movement/movement.cpp
        game/movement/movement.h
        game/movement/pathfinding.cpp
        game/movement/pathfinding.h
        game/property_names.h
        game/properties.cpp
        game/properties.h
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
        resources/image_loader.cpp
        resources/image_ref.h
        resources/message_types.h
        resources/resource_loader.cpp
        resources/resources.cpp
        resources/resources.h
        resources/resource_messages.cpp
        resources/resource_messages.h
        resources/sound_loader.cpp
        resources/sound_ref.h
        resources/view_def.h
        resources/view_def_serialisation.h
        ui/controls.cpp
        ui/controls.h
        ui/loop.cpp
        ui/ui.h
        ui/ui_window.cpp
        view/combat/battle_viewer.cpp
        view/combat/battle_viewer.h
        view/combat/battle_window.cpp
        view/combat/battle_window.h
        view/combat/combat_view.cpp
        view/combat/combat_view.h
        view/audio_renderer.cpp
        view/audio_renderer.h
        view/game_view.cpp
        view/ghost.cpp
        view/level_renderer.cpp
        view/level_renderer.h
        view/level_view.cpp
        view/level_window.cpp
        view/level_window.h
        view/map_window.cpp
        view/map_window.h
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
        view/tile_painter.cpp
        view/tile_painter.h
        view/unit_info_window.cpp
        view/unit_info_window.h
        view/unit_renderer.cpp
        view/unit_renderer.h
        view/view.h
        view/view_updater.cpp
        view/view_updater.h
)

source_group(Sources REGULAR_EXPRESSION cpp|h)
source_group(Sources\\ai REGULAR_EXPRESSION /ai/)
source_group(Sources\\audio REGULAR_EXPRESSION /audio/)
source_group(Sources\\chat REGULAR_EXPRESSION /chat/)
source_group(Sources\\game REGULAR_EXPRESSION /game/)
source_group(Sources\\game\\combat REGULAR_EXPRESSION /game/combat/)
source_group(Sources\\game\\generation REGULAR_EXPRESSION /game/generation/)
source_group(Sources\\game\\movement REGULAR_EXPRESSION /game/movement/)
source_group(Sources\\graphics REGULAR_EXPRESSION /graphics/)
source_group(Sources\\resources REGULAR_EXPRESSION /resources/)
source_group(Sources\\ui REGULAR_EXPRESSION /ui/)
source_group(Sources\\view REGULAR_EXPRESSION /view/)
source_group(Sources\\view\\combat REGULAR_EXPRESSION /view/combat/)
add_library(hex ${srcs})
target_compile_options(hex PUBLIC ${gen_opts})

if (COMMAND cotire)
    cotire(hex)
endif()
