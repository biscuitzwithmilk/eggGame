# Minimal CMake file to build the csvutils library
add_library(csvutils STATIC
    src/getSections.cpp
)

target_include_directories(csvutils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/thirdParty/jsonutils/include
)
