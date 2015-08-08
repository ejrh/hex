cmake_minimum_required(VERSION 2.8)
project(hex)

# Cotire, used for precompiled header support
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(cotire OPTIONAL)

# Find dependencies
set(BOOST_ROOT D:/lib/boost_1_58_0)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system thread program_options filesystem)

find_package(Threads)

set(gen_libs SDL2 SDL2_ttf SDL2_image SDL2main ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} mikmod)

# Set general options
if(MSVC)
    set(gen_opts "/W3")
    add_definitions(-D_WIN32_WINNT=0x0501)
    add_definitions("/wd4996")
    set(CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:\"msvcrt.lib\")
endif(MSVC)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(gen_opts "-Wall" "-g")
endif(CMAKE_COMPILER_IS_GNUCXX)

include_directories("src")

add_subdirectory(src)
