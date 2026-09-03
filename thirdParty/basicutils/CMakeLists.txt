# Minimal CMake file to build the csvutils library
add_library(basicutils STATIC
    src/basicutils.cpp
)

target_include_directories(basicutils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
