#include "Enemy.h"
#include <cmath>
#include <algorithm>

// -----------------------------------------------------------------------
void Enemy::update(float dt, Physics2D& physics) {
    if (!active || bodyId < 0) return;
    Body2D& b = physics.body(bodyId);

    if (type == EnemyType::SENTINEL) {
        // Patrol back and forth between patrolA and patrolB.
        // The sentinel ignores global gravity (noGravity=true) and sticks
        // to the surface defined by its patrol path.
        glm::vec2 target = movingToB ? patrolB : patrolA;
        glm::vec2 dir    = target - b.pos;
        float     dist   = glm::length(dir);

        if (dist < 0.15f) {
            movingToB = !movingToB;   // reverse
        } else {
            b.vel = glm::normalize(dir) * speed;
        }
    } else {
        // WRAITH: orbit around a fixed centre point.
        orbitAngle += orbitSpeed * dt;
        b.pos = orbitCenter + glm::vec2(
            std::cos(orbitAngle) * orbitRadius,
            std::sin(orbitAngle) * orbitRadius
        );
        b.vel = { 0.0f, 0.0f };   // position is kinematic, driven directly
    }
}

// -----------------------------------------------------------------------
bool updateEnemies(std::vector<Enemy>& enemies, Physics2D& physics,
                   int playerBodyId, float dt)
{
    bool killed = false;
    for (auto& e : enemies) {
        if (!e.active || e.bodyId < 0) continue;
        e.update(dt, physics);

        // Touch check vs player
        if (playerBodyId >= 0) {
            const Body2D& pb = physics.body(playerBodyId);
            const Body2D& eb = physics.body(e.bodyId);
            if (aabbOverlap(pb, eb))
                killed = true;
        }
    }
    return killed;
}
