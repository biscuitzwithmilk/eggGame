cmake_minimum_required(VERSION 3.16)
project(game_app LANGUAGES CXX)

file(GLOB GAME_SOURCES src/*.cpp)
add_executable(game_app ${GAME_SOURCES})

target_include_directories(game_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(game_app PRIVATE csvutils jsonutils readLanguages raylib GL m pthread dl rt X11)
