
#pragma once
#include "Texture.h"

class ResourceManager
{
public:
    static const Texture& GetTexture(const std::string& name, Texture::WrapMode wrap_mode);
};
