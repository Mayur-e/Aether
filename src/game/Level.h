#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Physics2D.h"
#include "PuzzleObjects.h"
#include "Enemy.h"
#include "Player.h"
#include "../graphics/Renderer.h"
#include "../graphics/Mesh.h"
#include "../graphics/GameCamera.h"

// Static geometry block (wall/platform).
struct Geometry {
    glm::vec2 pos;
    glm::vec2 size;
    glm::vec3 color;
};

// Screen-space tutorial hint text.
struct TextHint {
    glm::vec2   pos;
    std::string text;
};

// -----------------------------------------------------------------------
class Level {
public:
    Physics2D          physics;
    GravitySystem      gravity;
    Player             player;

    std::string        name;
    glm::vec2          spawnPoint   { 1.0f, 2.0f };
    glm::vec2          checkpointPos{ 1.0f, 2.0f }; // updated at CHECKPOINT tokens
    float              gravityScale = 1.0f;          // global gravity strength multiplier (L5)

    std::vector<Geometry>      walls;
    std::vector<PressurePlate> plates;     // formerly 'buttons'
    std::vector<Door>          doors;
    std::vector<GravityCube>   cubes;
    std::vector<GravitySwitch> switches;
    std::vector<GravityField>  fields;
    std::vector<ConduitValve>  valves;
    std::vector<Goal>          goals;
    std::vector<Spike>         spikes;
    std::vector<Enemy>         enemies;
    std::vector<TextHint>      texts;

    Mesh* cubeMesh = nullptr; // Shared mesh for all blocks

    // Load and parse a level text file.
    void loadFromFile(const std::string& path);

    // Reset player to the last checkpoint (called on death).
    void respawnPlayer();

    void update(float dt);
    void draw(Renderer& renderer, const GameCamera& camera);

    bool isComplete = false;

private:
    std::string filePath_; // remember for restart

    void buildHardcodedLevel1();
    void addWall(float x, float y, float w, float h,
                 float r = 0.15f, float g = 0.15f, float b = 0.20f);
};
