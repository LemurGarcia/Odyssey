cmake_minimum_required (VERSION 2.8)
project(Engine)

set(SRC
    Engine.cpp
    Engine.h
    VertexBuffer.cpp
    VertexBuffer.h
    Shader.cpp
    Shader.h
    Texture.cpp
    Texture.h
    Inputs.cpp
    Inputs.h
    Font.cpp
    Font.h
    Json.cpp
    Json.h
    ResourceManager.cpp
    ResourceManager.h
)

add_library(Engine STATIC ${SRC})
target_link_libraries(Engine glfw libglew_static)
