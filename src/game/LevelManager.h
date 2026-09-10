#pragma once
#include <map>
#include <string>
#include <memory>
#include "Level.h"

// -----------------------------------------------------------------------
// LevelManager
// Handles loading, storing, and transitioning between levels.
// -----------------------------------------------------------------------
class LevelManager {
public:
    std::map<std::string, std::string> levelPaths;
    std::unique_ptr<Level> currentLevel;
    
    void loadLevel(const std::string& name);
    void restartLevel();
    void update(float dt);
    void draw(Renderer& renderer, const GameCamera& camera);
    
    Mesh* sharedCubeMesh = nullptr;
    
private:
    std::string currentLevelName;
};
