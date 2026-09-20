cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
project(game_test LANGUAGES CXX)

add_subdirectory("thirdParty/jsonutils")
add_subdirectory("thirdParty/readLanguages")
add_subdirectory("thirdParty/csvutils")
add_subdirectory("thirdParty/basicutils")
# find_package(raylib REQUIRED PATHS thirdParty/raylib-6.0/build)
add_subdirectory("thirdParty/raylib-6.0")

# Build the game executable.
add_subdirectory(game/scripts)
