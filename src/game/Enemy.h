#pragma once
#include "Physics2D.h"
#include <glm/glm.hpp>
#include <vector>

// -----------------------------------------------------------------------
// Enemy.h — Sentinel and Fracture Wraith enemies for AETHER
// -----------------------------------------------------------------------

enum class EnemyType { SENTINEL, WRAITH };

struct Enemy {
    EnemyType type       = EnemyType::SENTINEL;
    int       bodyId     = -1;
    bool      active     = true;

    // Sentinel patrol
    glm::vec2 patrolA    { 0.0f, 0.0f };
    glm::vec2 patrolB    { 5.0f, 0.0f };
    float     speed      = 4.0f;
    bool      movingToB  = true;

    // Wraith orbit
    glm::vec2 orbitCenter { 0.0f, 0.0f };
    float     orbitRadius = 6.0f;
    float     orbitAngle  = 0.0f;       // radians, current angle
    float     orbitSpeed  = 1.0f;       // radians/sec

    void update(float dt, Physics2D& physics);
};

// Update all enemies. Returns true if the player was killed.
bool updateEnemies(std::vector<Enemy>& enemies, Physics2D& physics,
                   int playerBodyId, float dt);
