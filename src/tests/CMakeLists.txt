add_executable(command_test command_test.cpp)
target_link_libraries(command_test hexgame hexutil)
target_link_libraries(command_test ${gen_libs})
target_compile_options(command_test PUBLIC ${gen_opts})

add_executable(data_test data_test.cpp)
target_link_libraries(data_test hexview hexgame hexav hexutil)
target_link_libraries(data_test ${gen_libs} ${av_libs})
target_compile_options(data_test PUBLIC ${gen_opts})

add_executable(networking_test networking_test.cpp)
target_link_libraries(networking_test hexgame hexutil)
target_link_libraries(networking_test ${gen_libs})
target_compile_options(networking_test PUBLIC ${gen_opts})

add_subdirectory(auto)

add_subdirectory(ui)
