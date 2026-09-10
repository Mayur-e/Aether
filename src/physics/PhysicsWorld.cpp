#include "PhysicsWorld.h"

#include <algorithm>
#include <cmath>

// -----------------------------------------------------------------------
int PhysicsWorld::addBody(const RigidBody& body)
{
    bodies_.push_back(body);
    return (int)bodies_.size() - 1;
}

void PhysicsWorld::removeBody(int index)
{
    if (index < 0 || index >= (int)bodies_.size()) return;
    std::swap(bodies_[(size_t)index], bodies_.back());
    bodies_.pop_back();
}

// -----------------------------------------------------------------------
void PhysicsWorld::step(float dt)
{
    integrate(dt);
    resolveCollisions();
}

// -----------------------------------------------------------------------
void PhysicsWorld::integrate(float dt)
{
    for (auto& b : bodies_)
    {
        if (b.mass == 0.0f) continue;   // static

        // Apply gravity
        b.velocity.y += GRAVITY * dt;

        // Clamp terminal velocity (prevents tunnelling at large dt)
        const float MAX_FALL = 50.0f;
        b.velocity.y = std::max(b.velocity.y, -MAX_FALL);

        // Integrate position
        b.position += b.velocity * dt;

        // Simple floor plane at y = 0 (ground collision)
        if (b.position.y - b.halfExtents.y < 0.0f)
        {
            b.position.y = b.halfExtents.y;
            b.velocity.y = 0.0f;
            b.grounded   = true;
        }
        else
        {
            b.grounded = false;
        }
    }
}

// -----------------------------------------------------------------------
void PhysicsWorld::resolveCollisions()
{
    // Brute-force O(n²) pairwise AABB check
    for (size_t i = 0; i < bodies_.size(); ++i)
    {
        for (size_t j = i + 1; j < bodies_.size(); ++j)
        {
            RigidBody& a = bodies_[i];
            RigidBody& b = bodies_[j];

            // Skip static vs static
            if (a.mass == 0.0f && b.mass == 0.0f) continue;

            AABB aabb_a = a.aabb();
            AABB aabb_b = b.aabb();

            if (!aabb_a.intersects(aabb_b)) continue;

            // Minimum penetration axis resolution
            float px = aabb_a.penetrationX(aabb_b);
            float py = aabb_a.penetrationY(aabb_b);
            float pz = aabb_a.penetrationZ(aabb_b);

            glm::vec3 normal(0.0f);
            float     depth = 0.0f;

            if (px <= py && px <= pz)
            {
                depth  = px;
                normal = { a.position.x < b.position.x ? -1.0f : 1.0f, 0, 0 };
            }
            else if (py <= px && py <= pz)
            {
                depth  = py;
                normal = { 0, a.position.y < b.position.y ? -1.0f : 1.0f, 0 };
            }
            else
            {
                depth  = pz;
                normal = { 0, 0, a.position.z < b.position.z ? -1.0f : 1.0f };
            }

            // Separate based on mass ratio
            if (a.mass == 0.0f)
            {
                b.position += normal * depth;
                b.velocity -= glm::dot(b.velocity, normal) * normal;
            }
            else if (b.mass == 0.0f)
            {
                a.position -= normal * depth;
                a.velocity -= glm::dot(a.velocity, normal) * normal;
            }
            else
            {
                float totalMass = a.mass + b.mass;
                a.position -= normal * depth * (b.mass / totalMass);
                b.position += normal * depth * (a.mass / totalMass);

                // Simple inelastic velocity correction
                glm::vec3 relVel = a.velocity - b.velocity;
                float     vn     = glm::dot(relVel, normal);
                if (vn < 0.0f)   // approaching
                {
                    const float restitution = 0.3f;
                    float j = -(1.0f + restitution) * vn / (1.0f/a.mass + 1.0f/b.mass);
                    a.velocity += (j / a.mass) * normal;
                    b.velocity -= (j / b.mass) * normal;
                }
            }
        }
    }
}
