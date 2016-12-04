add_subdirectory(hexutil)

add_subdirectory(hexav)

add_subdirectory(hexgame)

add_subdirectory(hexview)

add_executable(hexmain main.cpp)
target_link_libraries(hexmain hexview hexgame hexav hexutil)
target_link_libraries(hexmain ${gen_libs} ${av_libs})
target_compile_options(hexmain PUBLIC ${gen_opts})

add_subdirectory(tests)
