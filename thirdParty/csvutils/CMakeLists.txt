# Minimal CMake file to build the csvutils library
add_library(csvutils STATIC
    src/getSections.cpp
)

target_link_libraries(csvutils PUBLIC jsonutils basicutils raylib_static)

target_include_directories(csvutils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
