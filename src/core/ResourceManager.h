#pragma once
#include "../graphics/Shader.h"
#include <map>
#include <string>
#include <memory>

// -----------------------------------------------------------------------
// ResourceManager
// Cache for shaders and textures to prevent duplicate loading.
// -----------------------------------------------------------------------
class ResourceManager {
public:
    static Shader* getShader(const std::string& name, const std::string& vertPath, const std::string& fragPath);
    // TODO: Add Texture loading when we have textures
    static void clear();

private:
    static std::map<std::string, std::unique_ptr<Shader>> shaders_;
};
