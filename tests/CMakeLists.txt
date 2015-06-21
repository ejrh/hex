add_executable(colour_key_conversion_test colour_key_conversion_test.c)
target_link_libraries(colour_key_conversion_test SDL2 SDL2_ttf)
target_compile_options(colour_key_conversion_test PUBLIC "-Wall" "-g")

add_executable(command_test command_test.cpp)
target_link_libraries(command_test hex)
target_link_libraries(command_test SDL2 SDL2_ttf boost_system)
target_compile_options(command_test PUBLIC "-Wall" "-g")

add_executable(data_test data_test.cpp)
target_link_libraries(data_test hex)
target_link_libraries(data_test SDL2 SDL2_image SDL2_ttf boost_system)
target_compile_options(data_test PUBLIC "-Wall" "-g")

add_executable(noise_test noise_test.cpp)
target_link_libraries(noise_test hex)
target_link_libraries(noise_test SDL2 SDL2_image SDL2_ttf boost_system)
target_compile_options(noise_test PUBLIC "-Wall" "-g")

add_executable(pathfinding_test pathfinding_test.cpp)
target_link_libraries(pathfinding_test hex)
target_link_libraries(pathfinding_test SDL2 SDL2_image SDL2_ttf boost_system)
target_compile_options(pathfinding_test PUBLIC "-Wall" "-g")
