#include "LevelManager.h"
#include <iostream>

void LevelManager::loadLevel(const std::string& name) {
    currentLevelName = name;
    
    currentLevel = std::make_unique<Level>();
    currentLevel->cubeMesh = sharedCubeMesh;
    
    // Fallback if not found in map, or use map if available later
    std::string path = "";
    if (levelPaths.find(name) != levelPaths.end()) {
        path = levelPaths[name];
    }
    
    currentLevel->loadFromFile(path);
}

void LevelManager::restartLevel() {
    if (!currentLevelName.empty()) {
        loadLevel(currentLevelName);
    }
}

void LevelManager::update(float dt) {
    if (currentLevel) {
        currentLevel->update(dt);
    }
}

void LevelManager::draw(Renderer& renderer, const GameCamera& camera) {
    if (currentLevel) {
        currentLevel->draw(renderer, camera);
    }
}
