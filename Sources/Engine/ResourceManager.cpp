
#include "ResourceManager.h"
#include <global.h>
#include <map>

const Texture& ResourceManager::GetTexture(const std::string& name, Texture::WrapMode wrap_mode)
{
    static std::map<std::string, Texture> s_textures;
    auto it = s_textures.find(name);
    if (it == s_textures.end())
        it = s_textures.emplace(name, Texture(name, wrap_mode)).first;
    //PV_ASSERT(pair.second);
    return it->second;
}
