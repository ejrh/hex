add_subdirectory(hex)

add_executable(hexmain main.cpp)
target_link_libraries(hexmain hex)
target_link_libraries(hexmain ${gen_libs})
target_compile_options(hexmain PUBLIC ${gen_opts})

add_subdirectory(tests)
