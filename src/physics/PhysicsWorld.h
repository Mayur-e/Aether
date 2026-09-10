#pragma once

#include "AABB.h"
#include <glm/glm.hpp>
#include <vector>

// -----------------------------------------------------------------------
// RigidBody — a simulated physics object
// -----------------------------------------------------------------------
struct RigidBody
{
    glm::vec3 position   = { 0, 0, 0 };
    glm::vec3 velocity   = { 0, 0, 0 };
    glm::vec3 halfExtents = { 0.5f, 0.5f, 0.5f };

    float mass       = 1.0f;    // 0 = static (immovable)
    bool  grounded   = false;

    AABB aabb() const { return AABB(position, halfExtents); }
};

// -----------------------------------------------------------------------
// PhysicsWorld — integrates gravity + velocity, resolves AABB collisions
//
// Usage:
//   PhysicsWorld world;
//   int id = world.addBody({ ... });
//   world.step(dt);
//   glm::vec3 pos = world.getBody(id).position;
// -----------------------------------------------------------------------
class PhysicsWorld
{
public:
    static constexpr float GRAVITY = -9.81f;

    // Add a body; returns its index
    int  addBody(const RigidBody& body);

    // Remove a body by index (swaps with last)
    void removeBody(int index);

    // Access
    RigidBody&       getBody(int index)       { return bodies_[(size_t)index]; }
    const RigidBody& getBody(int index) const { return bodies_[(size_t)index]; }
    int              bodyCount()        const { return (int)bodies_.size(); }

    // Integrate one time step (dt seconds)
    void step(float dt);

private:
    std::vector<RigidBody> bodies_;

    void integrate(float dt);
    void resolveCollisions();
};
