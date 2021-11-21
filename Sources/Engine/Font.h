
#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <string>

class Font
{
public:
    static void Initialize();
    static void Deinitialize();
    static void Resize(float size);
    static void Render(const std::wstring &text, const glm::vec2 &pos);
};
