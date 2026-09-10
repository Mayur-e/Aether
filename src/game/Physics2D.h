#pragma once
#include <glm/glm.hpp>
#include <vector>

// -----------------------------------------------------------------------
// Body2D — a 2-D axis-aligned rigid body
//
// mass == 0  →  static (never moved by physics)
// noGravity  →  kinematic (has velocity, not affected by global gravity)
// -----------------------------------------------------------------------
struct Body2D {
    glm::vec2 pos               { 0.0f, 0.0f };
    glm::vec2 vel               { 0.0f, 0.0f };
    glm::vec2 half              { 0.5f, 0.5f };   // AABB half-extents
    float     mass              = 1.0f;
    bool      onGround          = false;
    bool      noGravity         = false;           // set true for Sentinels / triggers
    bool      active            = true;
    // Per-body gravity override — set by applyGravityFields() each frame.
    glm::vec2 gravityOverride   { 0.0f, 0.0f };
    bool      hasGravityOverride = false;
};

// -----------------------------------------------------------------------
// Standalone AABB helpers (used by Level::update for trigger checks)
// -----------------------------------------------------------------------
bool  aabbOverlap(const Body2D& a, const Body2D& b);
float aabbPenX   (const Body2D& a, const Body2D& b);
float aabbPenY   (const Body2D& a, const Body2D& b);

// Point-vs-AABB
bool  aabbContains(const Body2D& b, glm::vec2 pt);

// -----------------------------------------------------------------------
// Physics2D — simple 2-D Euler-integration physics with AABB collisions.
//
// Supports gravity in any of the 4 cardinal directions via the
// `gravity` member (set by GravitySystem each frame).
//
// Step order:
//   1. Integrate velocity (apply gravity, clamp speed, move position)
//   2. Resolve all pairwise AABB collisions
//   3. Update `onGround` flags based on collision normals
// -----------------------------------------------------------------------
class Physics2D {
public:
    glm::vec2 gravity  { 0.0f, -20.0f };  // updated externally by GravitySystem
    float     maxSpeed = 12.0f;            // 12 m/s terminal velocity (design doc)

    // Body management — returns stable integer ID.
    // IDs are valid until clear() is called.
    int  addBody  (const Body2D& b);
    void removeBody(int id);               // marks inactive, does not shrink vector
    void clear    ();                      // remove all bodies

    Body2D&       body(int id)       { return bodies_[(size_t)id]; }
    const Body2D& body(int id) const { return bodies_[(size_t)id]; }
    int  numBodies() const           { return (int)bodies_.size(); }

    // Advance one time-step.
    void step(float dt);

private:
    std::vector<Body2D> bodies_;

    void integrate       (float dt);
    void resolveCollisions();
};
