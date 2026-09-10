---
name: physics-system
image:
  url: https://media.craftrpgs.com/containers/01a074ae-3136-72dc-a7f0-965bf56e0889/images/419fbdfe-000d-4b43-a8e0-9febe762321c
  generation:
    status: success
    startedAt: 2026-09-08T16:28:53.743Z
    finishedAt: 2026-09-08T16:29:10.896Z
    prompt: Abstract dramatic illustration of 2D physics collision, glowing
      wireframe AABB boxes colliding with minimum penetration axis highlighted,
      velocity and gravity vector arrows, capsule figure and violet crate
      mid-collision, dark technical void, dark sci-fi concept art, moody
      cinematic lighting, highly detailed game key art, no text, no UI, no
      watermark
$craft:
  referenceId: 01a074db-72c8-723d-ba1a-ae62295202c6
---

# Physics System

## Overview

AETHER uses a custom lightweight 2D physics system — no third-party engine. Everything is implemented from scratch in C++ using Newton's equations of motion and AABB collision.

This is a CGG project requirement: demonstrate physics understanding directly in code.

## Rigid Body Properties

| Property | Type | Description |
|---|---|---|
| `position` | vec2 | World-space centre |
| `velocity` | vec2 | m/s in world space |
| `halfExtents` | vec2 | AABB half-size |
| `mass` | float | 0 = static (immovable) |
| `grounded` | bool | True when resting on a surface in the current gravity direction |
| `isAffectedByGravity` | bool | False for Sentinels |
| `friction` | float | Surface deceleration |
| `gravityOverride` | optional<vec2> | Per-body gravity from a Gravity Field zone; empty = use global GravitySystem vector |

## Step-by-Step: The Simulation Step

1. Pull the current gravity vector from `GravitySystem` — a mid-blend lerped value applies automatically during a 0.4 s flip.
2. Apply gravity to every non-static body with `isAffectedByGravity == true`; a body inside a Gravity Field uses its per-body `gravityOverride` instead of the global vector.
3. Clamp velocity to terminal velocity (`MAX_SPEED`).
4. Integrate: `position += velocity * dt`, with `dt` clamped to 0.05 s.
5. Broadphase: only dynamic-vs-static and dynamic-vs-dynamic AABB pairs (static-vs-static never collides).
6. For each overlapping pair, resolve along the minimum-penetration axis and remove the velocity component along the collision normal.
7. Re-evaluate `grounded` — the last resolved contact along the current gravity normal means the body rests on that surface.
8. Apply friction to grounded bodies along the gravity tangent.

## Integration (Euler method)

```cpp
constexpr float MAX_SPEED = 12.0f;   // m/s terminal velocity, tunable per zone

// Apply gravity
body.velocity += gravityVector * GRAVITY_STRENGTH * dt;

// Clamp terminal velocity
body.velocity = clamp(body.velocity, -MAX_SPEED, MAX_SPEED);

// Integrate position
body.position += body.velocity * dt;
```

## AABB Collision Detection

```
Two boxes overlap if:
  |a.centre.x - b.centre.x| < a.halfX + b.halfX  AND
  |a.centre.y - b.centre.y| < a.halfY + b.halfY
```

## Collision Resolution — Minimum Penetration Axis

```
Compute penetration on each axis:
  px = (a.halfX + b.halfX) - |a.x - b.x|
  py = (a.halfY + b.halfY) - |a.y - b.y|

Resolve along the axis with smallest penetration:
  if px < py: push apart on X axis
  else:       push apart on Y axis

Velocity correction: remove velocity component along collision normal.
A body is grounded when the resolved axis is the gravity axis and the
contacted body is static.
```

## Ground Detection

"Grounded" is defined relative to the current gravity direction:
- Gravity DOWN: grounded if bottom face touching a static body from above
- Gravity UP: grounded if top face touching a static body from below
- Gravity LEFT: grounded if left face touching a static body from the right
- Gravity RIGHT: grounded if right face touching a static body from the left

Implementation: after collision resolution, check if last resolved collision was in the gravity direction. If so, `isGrounded = true`. This is what lets the player stand on ceilings and walls — gravity never rotates the world, the grounded test rotates with it.

## Friction

```cpp
if (body.isGrounded) {
    float decel = friction * dt;
    float tangentialSpeed = dot(body.velocity, gravityTangent);
    tangentialSpeed = sign(tangentialSpeed) * max(0, abs(tangentialSpeed) - decel);
    body.velocity = gravityNormal * dot(body.velocity, gravityNormal)
                  + gravityTangent * tangentialSpeed;
}
```

## C++ Implementation Notes

```cpp
struct RigidBody {
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec2 halfExtents;
    float mass;                       // 0 = static
    bool grounded;
    bool isAffectedByGravity;
    float friction;
    std::optional<glm::vec2> gravityOverride;   // set by GravityField zones
};

struct PhysicsWorld {
    std::vector<RigidBody> bodies;
    GravitySystem gravity;

    void step(float rawDt) {
        const float dt = glm::min(rawDt, 0.05f);          // no spiral of death
        glm::vec2 g = gravity.gravityVector();            // mid-blend lerped

        for (RigidBody& body : bodies) {
            if (body.mass == 0.0f) continue;              // static
            glm::vec2 grav = body.gravityOverride.value_or(g);
            if (body.isAffectedByGravity) body.velocity += grav * dt;
            body.velocity = glm::clamp(body.velocity, -MAX_SPEED, MAX_SPEED);
            body.position += body.velocity * dt;
        }

        resolveAABBPairs();                               // minimum-penetration axis
        updateGrounded(gravity.gravityNormal());          // contact along normal → grounded
        applyFriction(dt);                                // grounded bodies only
    }

private:
    void resolveAABBPairs();
    void updateGrounded(glm::vec2 gravityNormal);
    void applyFriction(float dt);
};
```

## Performance

- Brute-force O(n²) pairwise — adequate for ≤30 bodies per level
- All static platforms stored separately (only check dynamic vs static + dynamic vs dynamic)
- Step called once per frame with clamped `dt` (max 0.05s prevents spiral of death)

---
