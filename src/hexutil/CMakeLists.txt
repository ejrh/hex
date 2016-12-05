set(srcs
        basics/atom.cpp
        basics/atom.h
        basics/datum.cpp
        basics/datum.h
        basics/error.h
        basics/hexgrid.cpp
        basics/hexgrid.h
        basics/intset.h
        basics/io.h
        basics/noise.cpp
        basics/noise.h
        basics/objmap.h
        basics/point.h
        basics/properties.cpp
        basics/properties.h
        basics/string_vector.cpp
        basics/string_vector.h
        basics/vector2.h
        messaging/builtin_messages.cpp
        messaging/builtin_messages.h
        messaging/checksum.cpp
        messaging/checksum.h
        messaging/loader.cpp
        messaging/loader.h
        messaging/logger.cpp
        messaging/logger.h
        messaging/message_types.h
        messaging/message.cpp
        messaging/message.h
        messaging/message_h_template.h
        messaging/message_cpp_template.h
        messaging/publisher.cpp
        messaging/publisher.h
        messaging/queue.cpp
        messaging/queue.h
        messaging/receiver.cpp
        messaging/receiver.h
        messaging/serialiser.cpp
        messaging/serialiser.h
        messaging/writer.h
        networking/client.cpp
        networking/connection.cpp
        networking/networking.h
        networking/server.cpp
        scripting/builtin_instructions.cpp
        scripting/compiler.cpp
        scripting/execution.cpp
        scripting/scripting.h
)

source_group(Sources REGULAR_EXPRESSION cpp|h)
source_group(Sources\\basics REGULAR_EXPRESSION /basics/)
source_group(Sources\\messaging REGULAR_EXPRESSION /messaging/)
source_group(Sources\\networking REGULAR_EXPRESSION /networking/)
source_group(Sources\\scripting REGULAR_EXPRESSION /scripting/)
add_library(hexutil ${srcs})
target_compile_options(hexutil PUBLIC ${gen_opts})
if (COMMAND cotire)
    cotire(hexutil)
endif()
