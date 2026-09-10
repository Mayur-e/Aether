---
name: gravity-system
image:
  url: https://media.craftrpgs.com/containers/01a074ae-3136-72dc-a7f0-965bf56e0889/images/fa5caca0-fb83-4cdc-a2b6-4877855a0722
  generation:
    status: success
    startedAt: 2026-09-08T16:28:53.737Z
    finishedAt: 2026-09-08T16:29:04.563Z
    prompt: Abstract dramatic illustration of gravity manipulation, lone figure
      walking sideways on a wall as if floor, four glowing directional arrows
      blue purple green amber swirling around him, motion arcs and particle
      burst, dark void laboratory background, dark sci-fi concept art, moody
      cinematic lighting, highly detailed game key art, no text, no UI, no
      watermark
$craft:
  referenceId: 01a074db-72c8-723d-ba1a-a9ac70d8fb35
---

# Gravity System — Core Mechanic

## Overview

The defining mechanic of AETHER. Kael can redirect gravity to any of four cardinal directions. When gravity changes, the world doesn't rotate — only the direction of "down" changes. Objects fall in the new direction. You walk on whatever surface is now "below" you. Master the flip and every wall, ceiling and gap becomes a floor.

## Directions

| Direction | Vector | Key | Colour | Effect |
|---|---|---|---|---|
| **DOWN** | (0, -g) | F | blue (0.2, 0.5, 1.0) | Standard — floor is floor |
| **UP** | (0, +g) | R | purple (0.8, 0.3, 0.9) | Ceiling becomes floor |
| **LEFT** | (-g, 0) | Q | green (0.3, 0.9, 0.4) | Left wall becomes floor |
| **RIGHT** | (+g, 0) | E | amber (0.9, 0.6, 0.1) | Right wall becomes floor |

Where `g = 9.81 m/s²` (full strength) or varies per level/zone.

## Parameters

| Param | Value | Notes |
|---|---|---|
| Gravity strength `g` | 9.81 m/s² | world constant; overridden inside Gravity Fields |
| Blend duration | 0.4 s | gravity vector lerps linearly old → new |
| Cooldown | 0.8 s | between switch requests; input ignored while running |
| Player rotation | 0.4 s | quaternion slerp to face the new "up" |
| Camera roll | 2° | tilts toward new gravity, then eases back |
| Switch input | edge-triggered key press | holding the key does not re-fire |

## Step-by-Step: A Gravity Switch

1. You press **F / R / Q / E** — an edge-triggered input calls `GravitySystem::requestSwitch(dir)`.
2. If the 0.8 s cooldown is still draining, the request is ignored; the HUD arc shows the remaining time.
3. Otherwise `target = dir` and `blendT = 0` — the 0.4 s blend begins.
4. Each frame `gravityVector()` lerps old → new; your model slerps toward the new "up"; **velocity is untouched**, so momentum carries through the flip.
5. Blend completes: `current = target`. Grounded is re-evaluated **relative to the new gravity** — the surface now underneath you becomes your floor (ceiling under UP, wall under LEFT/RIGHT).
6. A particle pulse fires along the new gravity direction and a vignette edge pulse plays; cooldown starts draining.

## Transition Behaviour

1. `blendTimer` starts — 0.4 seconds
2. Gravity vector lerps linearly from old direction to new
3. Player velocity is **not** reset — momentum carries through the flip
4. Player model rotates to face new "up" using quaternion slerp
5. On transition complete, new gravity fully applied
6. Particle pulse emitted at player position in new gravity direction

The blend is what keeps a flip readable: an instant snap reads as a teleport and murders momentum cues; a 0.4 s lerp reads as a redirect you can aim with.

## Cooldown

- Default cooldown: **0.8 seconds** between switches
- HUD arc indicator (Bresenham circle arc) shows cooldown progress
- During cooldown: switch key input ignored

## Grounded Relative to Gravity

There is no "world down" for gameplay — only `gravityVector()`. Grounded checks, jump impulses and friction are all evaluated in the current gravity frame: normal `n = -normalize(g)`, tangent `t = perp(n)`. With gravity UP you stand on the ceiling and jump away from it; with gravity LEFT you stand on the left wall. The same code path runs for all four directions — the world never rotates, only the input/contact frame does (see ground detection in `physics-system`). Your Aether burn mark glows brighter while grounded in any non-DOWN direction, keeping the "which way is up" read always on screen.

## Gravity Fields (Zone Override)

Inside a `GravityField` AABB:
- Field's gravity vector replaces global gravity for any body inside it
- Player is aware: Aether burn mark glows when inside a field
- On exit: global gravity immediately resumes (no blend — snaps)

Fields feed bodies a per-body `gravityOverride` (see Rigid Body Properties in `physics-system`); they do not mutate global `GravitySystem` state.

## Visual Feedback

1. **Gravity indicator** (HUD bottom-left): animated arrow (↓ ↑ ← →)
2. **Cooldown ring**: Bresenham arc drawn around indicator, fills as cooldown recovers
3. **Player rotation**: body rotates toward new "up" over 0.4 seconds
4. **Particle pulse**: 24 particles burst outward from player in new gravity direction
5. **Camera tilt**: subtle 2° camera roll toward new gravity (then eases back)
6. **Screen vignette flash**: brief dark edge pulse on switch

## C++ Implementation Notes

```cpp
// glm::vec2 dirVector(GravityDirection): DOWN→(0,-1) UP→(0,+1) LEFT→(-1,0) RIGHT→(+1,0)
enum class GravityDirection { DOWN, UP, LEFT, RIGHT };

struct GravitySystem {
    GravityDirection current;
    GravityDirection target;
    float blendT;          // 0.0 → 1.0 over blendDuration
    float blendDuration;   // 0.4 s
    float cooldown;        // seconds remaining before next switch allowed
    float cooldownMax;     // 0.8 s
    bool isGrounded;       // relative to current gravity — set by PhysicsWorld

    glm::vec2 gravityVector() const {                    // mid-blend lerped vector
        return glm::mix(dirVector(current), dirVector(target), blendT);
    }
    glm::vec2 gravityNormal()  const { return -glm::normalize(gravityVector()); }
    glm::vec2 gravityTangent() const { return glm::vec2(-gravityNormal().y, gravityNormal().x); }

    void requestSwitch(GravityDirection dir) {
        if (cooldown > 0.0f || dir == current) return;   // edge-triggered, cooldown-gated
        target = dir;
        blendT = 0.0f;
        cooldown = cooldownMax;
    }

    void update(float dt) {
        if (blendT < 1.0f) {
            blendT = glm::min(1.0f, blendT + dt / blendDuration);
            if (blendT >= 1.0f) current = target;
        }
        cooldown = glm::max(0.0f, cooldown - dt);
    }
};
```

Physics application — inside `PhysicsWorld::integrate()`:
```cpp
glm::vec2 grav = gravitySystem.gravityVector();
body.velocity.x += grav.x * dt;
body.velocity.y += grav.y * dt;
```

Grounded and friction then operate on the current gravity frame (`gravityNormal()` / `gravityTangent()`), never world axes — see physics-system.

---
