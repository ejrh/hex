add_subdirectory(hex)

add_executable(hexmain main.cpp)
target_link_libraries(hexmain hex)
target_link_libraries(hexmain SDL2 SDL2_image SDL2_ttf boost_system pthread)
target_compile_options(hexmain PUBLIC "-Wall" "-g")
