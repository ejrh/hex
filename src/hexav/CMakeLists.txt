set(srcs
        audio/audio.cpp
        audio/audio.h
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
        resources/paint.h
        resources/paint.cpp
        resources/paint_instructions.cpp
        resources/resource_loader.cpp
        resources/resource_loader.h
        resources/resources.cpp
        resources/resources.h
        resources/resource_messages.cpp
        resources/resource_messages.h
        resources/sound_loader.cpp
        resources/sound_ref.h
        ui/controls.cpp
        ui/controls.h
        ui/loop.cpp
        ui/ui.h
        ui/ui_window.cpp
)

source_group(Sources REGULAR_EXPRESSION cpp|h)
source_group(Sources\\audio REGULAR_EXPRESSION /audio/)
source_group(Sources\\graphics REGULAR_EXPRESSION /graphics/)
source_group(Sources\\ui REGULAR_EXPRESSION /ui/)
add_library(hexav ${srcs})
target_compile_options(hexav PUBLIC ${gen_opts})
if (COMMAND cotire)
    cotire(hexav)
endif()
