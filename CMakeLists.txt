cmake_minimum_required (VERSION 2.8)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(CMAKE_CXX_FLAGS_DEBUG "-std=c++14 -stdlib=libc++ -Wall -D_DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-std=c++14 -stdlib=libc++ -O3 -Wall")
    set(CMAKE_CXX_FLAGS "-std=c++14 -stdlib=libc++ -O3 -Wall")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE STRING "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE STRING "" FORCE)
set(GLFW_INSTALL        OFF CACHE STRING "" FORCE)
add_subdirectory(3rdparty/glfw)
include_directories(3rdparty/glfw/include)

# glew
add_definitions(-DGLEW_STATIC)
option(glew-cmake_BUILD_SHARED "" OFF)
add_subdirectory(3rdparty/glew)
include_directories(3rdparty/glew/include 3rdparty/glew/src)

# glm
include_directories(3rdparty/glm)

# stb
include_directories(3rdparty/stb)

# rapidjson
set(RAPIDJSON_BUILD_DOC         OFF CACHE STRING "" FORCE)
set(RAPIDJSON_BUILD_EXAMPLES    OFF CACHE STRING "" FORCE)
set(RAPIDJSON_BUILD_TESTS       OFF CACHE STRING "" FORCE)
add_subdirectory(3rdparty/rapidjson)
include_directories(3rdparty/rapidjson/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Sources)