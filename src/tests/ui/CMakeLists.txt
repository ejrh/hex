add_executable(colour_key_conversion_test colour_key_conversion_test.c)
target_link_libraries(colour_key_conversion_test ${av_libs})
target_compile_options(colour_key_conversion_test PUBLIC ${gen_opts})

add_executable(image_test image_test.cpp)
target_link_libraries(image_test hexview hexgame hexav hexutil)
target_link_libraries(image_test ${gen_libs} ${av_libs})
target_compile_options(image_test PUBLIC ${gen_opts})

add_executable(noise_test noise_test.cpp)
target_link_libraries(noise_test hexav hexutil)
target_link_libraries(noise_test ${gen_libs} ${av_libs})
target_compile_options(noise_test PUBLIC ${gen_opts})

add_executable(pathfinding_test pathfinding_test.cpp)
target_link_libraries(pathfinding_test hexview hexgame hexav hexutil)
target_link_libraries(pathfinding_test ${gen_libs} ${av_libs})
target_compile_options(pathfinding_test PUBLIC ${gen_opts})

add_executable(ui_test ui_test.cpp)
target_link_libraries(ui_test hexav hexutil)
target_link_libraries(ui_test ${gen_libs} ${av_libs})
target_compile_options(ui_test PUBLIC ${gen_opts})

add_executable(unit_info_test unit_info_test.cpp)
target_link_libraries(unit_info_test hexview hexgame hexav hexutil)
target_link_libraries(unit_info_test ${gen_libs} ${av_libs})
target_compile_options(unit_info_test PUBLIC ${gen_opts})

add_executable(unit_rendering_test unit_rendering_test.cpp)
target_link_libraries(unit_rendering_test hexview hexgame hexav hexutil)
target_link_libraries(unit_rendering_test ${gen_libs} ${av_libs})
target_compile_options(unit_rendering_test PUBLIC ${gen_opts})
