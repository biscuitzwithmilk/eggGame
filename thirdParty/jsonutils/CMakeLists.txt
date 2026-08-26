# Minimal CMake file to build the csvutils library
add_library(jsonutils STATIC
    src/jsonutils.cpp
)

target_include_directories(jsonutils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
