#include "Physics2D.h"
#include <algorithm>
#include <cmath>

// -----------------------------------------------------------------------
bool aabbOverlap(const Body2D& a, const Body2D& b) {
    return (std::abs(a.pos.x - b.pos.x) < a.half.x + b.half.x) &&
           (std::abs(a.pos.y - b.pos.y) < a.half.y + b.half.y);
}

float aabbPenX(const Body2D& a, const Body2D& b) {
    return (a.half.x + b.half.x) - std::abs(a.pos.x - b.pos.x);
}

float aabbPenY(const Body2D& a, const Body2D& b) {
    return (a.half.y + b.half.y) - std::abs(a.pos.y - b.pos.y);
}

bool aabbContains(const Body2D& b, glm::vec2 pt) {
    return (std::abs(pt.x - b.pos.x) <= b.half.x) &&
           (std::abs(pt.y - b.pos.y) <= b.half.y);
}

// -----------------------------------------------------------------------
int Physics2D::addBody(const Body2D& b) {
    bodies_.push_back(b);
    return (int)bodies_.size() - 1;
}

void Physics2D::removeBody(int id) {
    if (id >= 0 && id < (int)bodies_.size())
        bodies_[(size_t)id].active = false;
}

void Physics2D::clear() {
    bodies_.clear();
}

// -----------------------------------------------------------------------
void Physics2D::step(float dt) {
    integrate(dt);
    // Multiple solver iterations for stable stacking
    for (int k = 0; k < 8; ++k) {
        resolveCollisions();
    }
}

// -----------------------------------------------------------------------
void Physics2D::integrate(float dt) {
    for (auto& b : bodies_) {
        if (!b.active || b.mass == 0.0f) continue;

        if (!b.noGravity) {
            // Use per-body override (from a GravityField zone) if set.
            glm::vec2 g = b.hasGravityOverride ? b.gravityOverride : gravity;
            b.vel += g * dt;
        }

        // Clamp to terminal velocity
        float spd = glm::length(b.vel);
        if (spd > maxSpeed)
            b.vel = (b.vel / spd) * maxSpeed;

        b.pos    += b.vel * dt;
        b.onGround = false;   // reset; set again after collision resolution
    }
}

// -----------------------------------------------------------------------
void Physics2D::resolveCollisions() {
    const int n = (int)bodies_.size();

    // Gravity direction normalised — used for grounded detection.
    // A body is "on the ground" when a collision pushes it *against* gravity.
    glm::vec2 gravNorm = (glm::length(gravity) > 0.0f)
                       ? glm::normalize(gravity)
                       : glm::vec2(0.0f, -1.0f);

    for (int i = 0; i < n; ++i) {
        Body2D& a = bodies_[(size_t)i];
        if (!a.active || a.mass == 0.0f) continue;

        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            Body2D& b = bodies_[(size_t)j];
            if (!b.active) continue;
            if (!aabbOverlap(a, b)) continue;

            float px = aabbPenX(a, b);
            float py = aabbPenY(a, b);

            // Resolve along axis of minimum penetration
            glm::vec2 normal;
            float     depth;
            if (px <= py) {
                depth  = px;
                normal = { (a.pos.x < b.pos.x) ? -1.0f : 1.0f, 0.0f };
            } else {
                depth  = py;
                normal = { 0.0f, (a.pos.y < b.pos.y) ? -1.0f : 1.0f };
            }

            if (b.mass == 0.0f) {
                // Static body — push A out entirely
                a.pos += normal * depth;
                float vn = glm::dot(a.vel, normal);
                if (vn < 0.0f)
                    a.vel -= vn * normal;   // zero velocity component into surface

            } else {
                // Both dynamic — split proportionally to mass
                float totalMass = a.mass + b.mass;
                a.pos += normal * depth * (b.mass / totalMass);
                b.pos -= normal * depth * (a.mass / totalMass);

                glm::vec2 rv = a.vel - b.vel;
                float     vn = glm::dot(rv, normal);
                if (vn < 0.0f) {
                    const float e = 0.15f;   // coefficient of restitution
                    float j = -(1.0f + e) * vn / (1.0f / a.mass + 1.0f / b.mass);
                    a.vel += (j / a.mass) * normal;
                    b.vel -= (j / b.mass) * normal;
                }
            }

            // Grounded: collision normal opposes gravity direction
            // dot(normal, -gravNorm) > 0.7 means normal is within ~45° of "up"
            float groundCheck = glm::dot(normal, -gravNorm);
            if (groundCheck > 0.70f)
                a.onGround = true;
        }
    }
}
