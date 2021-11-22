cmake_minimum_required (VERSION 2.8)
project(Odyssey)

set(GAME_DIR ${PROJECT_SOURCE_DIR})

# detect operating system
message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "System processor is: ${CMAKE_SYSTEM_PROCESSOR}")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    add_definitions(-D_LINUX)
    set(PLATFORM ${GAME_DIR}/platform/linux.cpp)
    string(SUBSTRING ${CMAKE_SYSTEM_PROCESSOR} 0 3 proc)
    if (${proc} STREQUAL "arm")
        add_definitions(-D_GLES)
    endif()
endif()
if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    add_definitions(-D_DARWIN)
    set(PLATFORM ${GAME_DIR}/platform/mac.cpp)
endif()
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    add_definitions(-D_WINDOWS)
    set(PLATFORM ${GAME_DIR}/platform/win.cpp)
endif()

set(INCLUDE
    ${GAME_DIR}
    ${GAME_DIR}/include
)

set(SRC
    ${PLATFORM}
    main.cpp
    ${INCLUDE}
)

include_directories(${INCLUDE})

add_subdirectory(Engine)
add_subdirectory(Utils)
add_subdirectory(Game)

set(LINK
    ${GLFW_LIBRARIES}
    libglew_static
    glfw
    Engine
    Utils
    Game
)

add_executable(Odyssey ${SRC})
target_link_libraries(Odyssey ${LINK})