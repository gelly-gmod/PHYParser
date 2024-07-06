cmake_minimum_required(VERSION 3.28)
project(PHYParser)

option(BUILD_TEST "Builds the test program" ON)

set(CMAKE_CXX_STANDARD 20)

add_library(PHYParser STATIC
        src/PHY.cpp
        src/PHY.h
        src/format/phyheader_t.h
        src/format/compactsurfaceheader_t.h
        src/format/vector.h
)

target_include_directories(PHYParser PUBLIC src)

if (BUILD_TEST)
    add_subdirectory(test)
endif ()
