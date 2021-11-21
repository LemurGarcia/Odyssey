cmake_minimum_required (VERSION 2.8)
project(Utils)

set(SRC
    Log.cpp
    Errors.cpp
)

add_library(Utils STATIC ${SRC})
target_link_libraries(Utils)
