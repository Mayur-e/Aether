#include "ResourceManager.h"

std::map<std::string, std::unique_ptr<Shader>> ResourceManager::shaders_;

Shader* ResourceManager::getShader(const std::string& name, const std::string& vertPath, const std::string& fragPath) {
    auto it = shaders_.find(name);
    if (it != shaders_.end()) {
        return it->second.get();
    }
    
    // Create new shader
    auto shader = std::make_unique<Shader>(vertPath.c_str(), fragPath.c_str());
    Shader* ptr = shader.get();
    shaders_[name] = std::move(shader);
    return ptr;
}

void ResourceManager::clear() {
    shaders_.clear();
}
