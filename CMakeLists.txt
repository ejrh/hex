cmake_minimum_required(VERSION 3.1)
project(hex)

# Cotire, used for precompiled header support
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(cotire OPTIONAL)

# Find dependencies
set(BOOST_ROOT D:/lib/boost_1_58_0)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system log thread program_options filesystem regex)

find_package(Threads)

set(gen_libs ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
set(av_libs SDL2 SDL2_ttf SDL2_image SDL2_mixer SDL2main)

# Set general options
if(MSVC)
    set(gen_opts "/W3" "/MP")
    if(TREAT_WARNINGS_AS_ERRORS)
        set(gen_opts "${gen_opts}" "/WX")
    endif(TREAT_WARNINGS_AS_ERRORS)
    add_definitions(-D_WIN32_WINNT=0x0501)
    add_definitions("/wd4996")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:\"msvcrt.lib\"")
    STRING(REGEX REPLACE "/EHs" "/EHa" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif(MSVC)

if(CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    set(gen_opts "-Wall")
    if(TREAT_WARNINGS_AS_ERRORS)
        set(gen_opts "${gen_opts}" "-Werror")
    endif(TREAT_WARNINGS_AS_ERRORS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DHEX_BUILD_TYPE=DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -DHEX_BUILD_TYPE=RELEASE")
endif(CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))

set(CMAKE_CXX_STANDARD 11)

enable_testing()

include_directories("src")

add_subdirectory(src)
