cmake_minimum_required (VERSION 2.8)
project(Game)

set(SRC
    Level.cpp
    Level.h
    LevelRenderer.cpp
    LevelRenderer.h
)

add_library(Game STATIC ${SRC})
target_link_libraries(Game glfw libglew_static)
