set(srcs
        ai/ai.cpp
        ai/ai.h
        ai/ai_updater.cpp
        ai/ai_updater.h
        game/end_of_turn.cpp
        game/end_of_turn.h
        game/game.cpp
        game/game.h
        game/game_arbiter.cpp
        game/game_arbiter.h
        game/game_messages.cpp
        game/game_messages.h
        game/game_properties.cpp
        game/game_properties.h
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
        game/unit_stack.cpp
        game/visibility_map.cpp
        game/visibility_map.h
)

source_group(Sources REGULAR_EXPRESSION cpp|h)
source_group(Sources\\ai REGULAR_EXPRESSION /ai/)
source_group(Sources\\game REGULAR_EXPRESSION /game/)
source_group(Sources\\game\\combat REGULAR_EXPRESSION /game/combat/)
source_group(Sources\\game\\generation REGULAR_EXPRESSION /game/generation/)
source_group(Sources\\game\\movement REGULAR_EXPRESSION /game/movement/)
add_library(hexgame ${srcs})
target_compile_options(hexgame PUBLIC ${gen_opts})
if (COMMAND cotire)
    cotire(hexgame)
endif()
