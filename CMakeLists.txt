cmake_minimum_required(VERSION 3.28)
project(PHYParser)

option(BUILD_PHYPARSER_TEST "Builds the test program" ON)

set(CMAKE_CXX_STANDARD 20)

add_library(PHYParser STATIC
        src/PHY.cpp
        src/PHY.h
        src/format/phyheader_t.h
        src/format/compactsurfaceheader_t.h
        src/format/vector.h
        src/format/ivpcompactsurface_t.h
        src/semantics/PHYData.h
        src/semantics/Solid.cpp
        src/semantics/Solid.h
        src/format/ivpcompactledgenode_t.h
        src/format/ivpcompactledge_t.h
        src/format/ivpcompactedge_t.h
        src/format/ivpcompacttriangle_t.h
        src/exceptions/UnsupportedModelType.h
        src/format/modeltype.h
        src/format/bsp/dheader_t.h
        src/format/bsp/lump_t.h
        src/format/bsp/dphysmodel_t.h
        src/BSP.cpp
        src/BSP.h
        src/format/ivpcompactmopp_t.h
        src/format/moppsurfaceheader_t.h
        src/format/bsp/dphysdisp_t.h
)

target_include_directories(PHYParser PUBLIC src)

if (BUILD_PHYPARSER_TEST)
    add_subdirectory(test)
endif ()
