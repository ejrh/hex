add_executable(battle_test battle_test.cpp)
target_link_libraries(battle_test hex)
target_link_libraries(battle_test ${gen_libs})
target_compile_options(battle_test PUBLIC ${gen_opts})

add_executable(colour_key_conversion_test colour_key_conversion_test.c)
target_link_libraries(colour_key_conversion_test ${gen_libs})
target_compile_options(colour_key_conversion_test PUBLIC ${gen_opts})

add_executable(command_test command_test.cpp)
target_link_libraries(command_test hex)
target_link_libraries(command_test ${gen_libs})
target_compile_options(command_test PUBLIC ${gen_opts})

add_executable(data_test data_test.cpp)
target_link_libraries(data_test hex)
target_link_libraries(data_test ${gen_libs})
target_compile_options(data_test PUBLIC ${gen_opts})

add_executable(hexgrid_test hexgrid_test.cpp)
target_link_libraries(hexgrid_test hex)
target_link_libraries(hexgrid_test ${gen_libs})
target_compile_options(hexgrid_test PUBLIC ${gen_opts})

add_executable(image_test image_test.cpp)
target_link_libraries(image_test hex)
target_link_libraries(image_test ${gen_libs})
target_compile_options(image_test PUBLIC ${gen_opts})

add_executable(movement_test movement_test.cpp)
target_link_libraries(movement_test hex)
target_link_libraries(movement_test ${gen_libs})
target_compile_options(movement_test PUBLIC ${gen_opts})

add_executable(networking_test networking_test.cpp)
target_link_libraries(networking_test hex)
target_link_libraries(networking_test ${gen_libs})
target_compile_options(networking_test PUBLIC ${gen_opts})

add_executable(noise_test noise_test.cpp)
target_link_libraries(noise_test hex)
target_link_libraries(noise_test ${gen_libs})
target_compile_options(noise_test PUBLIC ${gen_opts})

add_executable(pathfinding_test pathfinding_test.cpp)
target_link_libraries(pathfinding_test hex)
target_link_libraries(pathfinding_test ${gen_libs})
target_compile_options(pathfinding_test PUBLIC ${gen_opts})

add_executable(string_utils_test string_utils_test.cpp)
target_link_libraries(string_utils_test hex)
target_link_libraries(string_utils_test ${gen_libs})
target_compile_options(string_utils_test PUBLIC ${gen_opts})

add_executable(unit_rendering_test unit_rendering_test.cpp)
target_link_libraries(unit_rendering_test hex)
target_link_libraries(unit_rendering_test ${gen_libs})
target_compile_options(unit_rendering_test PUBLIC ${gen_opts})
