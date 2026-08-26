# Minimal CMake file to build the csvutils library
add_library(readLanguages STATIC
    src/readLanguages.cpp
)
target_link_libraries(readLanguages PRIVATE csvutils)

target_include_directories(readLanguages PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
