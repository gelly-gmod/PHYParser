project(PHYParserTest)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(vendor/raylib)

add_executable(PHYParserTest src/main.cpp
        src/open-phy-file-dialog.cpp
        src/open-phy-file-dialog.h
        src/prompt.cpp
        src/prompt.h)
target_include_directories(
        PHYParserTest
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/raylib/src
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(PHYParserTest PHYParser raylib)

set_target_properties(PHYParserTest PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)