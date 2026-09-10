#pragma once

#include <glm/glm.hpp>
#include <algorithm>

// -----------------------------------------------------------------------
// AABB — Axis-Aligned Bounding Box
//
// Defined by a centre + half-extents (simpler math than min/max form).
//
// Usage:
//   AABB a({ 0,0,0 }, { 0.5f, 0.5f, 0.5f });
//   AABB b({ 1,0,0 }, { 0.5f, 0.5f, 0.5f });
//   if (a.intersects(b)) { ... }
// -----------------------------------------------------------------------
struct AABB
{
    glm::vec3 centre;        // world-space centre
    glm::vec3 halfExtents;   // half-sizes in each axis

    AABB() : centre(0.0f), halfExtents(0.5f) {}
    AABB(glm::vec3 centre, glm::vec3 halfExtents)
        : centre(centre), halfExtents(halfExtents) {}

    // Min / max corners
    glm::vec3 min() const { return centre - halfExtents; }
    glm::vec3 max() const { return centre + halfExtents; }

    // Overlap test
    bool intersects(const AABB& other) const
    {
        return (std::abs(centre.x - other.centre.x) < halfExtents.x + other.halfExtents.x) &&
               (std::abs(centre.y - other.centre.y) < halfExtents.y + other.halfExtents.y) &&
               (std::abs(centre.z - other.centre.z) < halfExtents.z + other.halfExtents.z);
    }

    // Signed penetration depth on a single axis (negative = separated)
    float penetrationX(const AABB& other) const
    {
        return (halfExtents.x + other.halfExtents.x) - std::abs(centre.x - other.centre.x);
    }
    float penetrationY(const AABB& other) const
    {
        return (halfExtents.y + other.halfExtents.y) - std::abs(centre.y - other.centre.y);
    }
    float penetrationZ(const AABB& other) const
    {
        return (halfExtents.z + other.halfExtents.z) - std::abs(centre.z - other.centre.z);
    }

    // Does the box contain a point?
    bool contains(const glm::vec3& p) const
    {
        glm::vec3 mn = min(), mx = max();
        return p.x >= mn.x && p.x <= mx.x &&
               p.y >= mn.y && p.y <= mx.y &&
               p.z >= mn.z && p.z <= mx.z;
    }
};
