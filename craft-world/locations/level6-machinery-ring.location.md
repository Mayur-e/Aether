---
name: level6-machinery-ring
image:
  url: https://media.craftrpgs.com/containers/01a074ae-3136-72dc-a7f0-965bf56e0889/images/45630a10-62f7-4a18-9497-2edd5bce2413
  generation:
    status: success
    startedAt: 2026-09-08T16:28:53.745Z
    finishedAt: 2026-09-08T16:29:06.180Z
    prompt: Wide establishing shot of colossal rotating machinery ring, massive
      flywheels and rotating platform arms around central pivot, orange hazard
      strips, steel girders and sparks, sense of orbital motion, dark sci-fi
      concept art, moody cinematic lighting, deep shadows with glowing accents,
      highly detailed game key art, no text, no UI, no watermark
$craft:
  referenceId: 01a074db-72c8-723d-ba1a-a2d9b62b92e5
---

# Section F-6 — The Machinery Ring

> *Level 6: Gravity Machinery*
> *Theme: Rotating Platforms & T*R*T Hierarchy*

## Overview

The mechanical heart of the Institute — a ring of interconnected rotating platforms, gravity-powered elevators, and timing-based gates. Before the catastrophe this section *ran the facility*: gravity conduits powered everything. Now it runs in a broken loop, rotating platforms spinning in different directions, gates opening and closing on unpredictable cycles.

Timing matters here for the first time. The player must observe rhythms and act within windows.

## Visual Style

- **Palette:** Industrial iron-grey (#141414) with bright orange hazard lighting (#FF7A1A strips) and worn warning chevrons
- **Mood:** Loud machinery (imagined). Constant motion. The environment itself is the enemy.
- **Depth layers:**
  - Back: huge dormant flywheels and conduit banks, slow ambient rotation
  - Mid: the platform ring itself — rotating arms, oscillating slabs, timed gates
  - Front: hazard-light strobes, sparks on pivot bearings, glow markers showing pivot centres
- **Signature visual:** Rotating platform arms — long rectangular meshes that rotate around a fixed centre pivot. When gravity is changed, the direction of their rotation *relative to the player* changes too.

## Mechanics Introduced

### Moving Platforms
- **Rotating Platform:** Rotates around a pivot point at fixed angular velocity
  - Transform: `T_pivot * R(time * speed) * T_arm_offset` — a T*R*T hierarchy
  - Player inherits platform velocity when standing on it (relative velocity tracking, see CGG note)
- **Oscillating Platform:** Moves back and forth on a fixed axis
  - `position = basePos + direction * sin(time * frequency) * amplitude`
- **Timed Gate:** On/off based on a period timer — open for `openDuration`, closed for `closeDuration`

## Geometry

```
World bounds: X [0, 60], Y [0, 20]
Gravity start: DOWN

Static Platforms:
  - Start ledge: pos(0,0), size(6,1)
  - End ledge:   pos(54,0), size(6,1)

Rotating Platforms (pivot, arm length, speed deg/sec):
  - RotPlat A: pivot(12,5), arm 4.0, speed 30  -- slow, training
  - RotPlat B: pivot(24,10), arm 5.0, speed 45 -- requires UP gravity to ride from above
  - RotPlat C: pivot(36,5), arm 6.0, speed 60  -- fast, must time jump between arms
  - RotPlat D: pivot(48,12), arm 4.0, speed 30 -- final stretch

Oscillating Platforms:
  - OscPlat A: base(18,3), amplitude 4, axis Y, frequency 0.5
  - OscPlat B: base(42,8), amplitude 5, axis X, frequency 0.8

Timed Gates (block passage until open):
  - Gate 1: pos(20,0), period 4s, open 2s
  - Gate 2: pos(44,0), period 6s, open 2s

Gravity Fields (orientation helpers):
  - Field [UP]: pos(22,6), size(6,8)   -- lifts player to ride RotPlat B from above

Objects:
  - ARIA node: pos(2,1)
  - Gravity Switch [UP]: pos(10,1)
  - Gravity Switch [DOWN]: pos(30,1)
  - Goal: pos(58,1)

Enemies:
  - Sentinel-Swarm: three drones patrolling between pos(30,0) and pos(44,0)
    Must all be grounded-knocked to pass
    
Checkpoint: pos(26,11) -- on RotPlat B upper surface
```

## Narrative

The machinery ring log reveals what the Core actually did: it didn't just break gravity. It *compressed* the Aether field into the Institute building itself. The building is now a closed gravity ecosystem — stable, but completely sealed.

To get out, the Core must be shut down from the inside. Which means Kael must reach it.

ARIA: *"I should have told you. The shutdown terminal can only be activated by the person who started the sequence. I am... sorry, Kael."*

## Physics Demonstration (CGG Note)

> **T*R*T transformation hierarchy for rotating platforms** — Each platform is a parent-pivot transform applied left-to-right:

```cpp
// arm origin placed in the pivot's frame: pivot -> rotate -> arm offset
float theta = glm::radians(speedDegPerSec) * t;
glm::mat4 M = glm::translate(pivot)                       // T
            * glm::rotate(theta, {0.f, 0.f, 1.f})         // R
            * glm::translate(armOffset);                  // T
```

> The matrix product `M = T * R * T` is the hierarchy: the arm's local origin is expressed in the pivot frame, so the whole sub-tree inherits the pivot's rotation — the CG concept of hierarchical transformation trees, applied every frame to every moving platform.

> **Velocity inheritance** — A player standing on a moving platform inherits `platformVelocity = (platformWorldPos(t) - platformWorldPos(t - dt)) / dt`, added to their own velocity each physics step. This is the parent-frame delta: the child (player) moves with the parent (platform) without altering its own local state.

> **Projection** — Level 6 is the first level where the camera uses a slight perspective tilt (`glm::perspective` with 60° FOV + mild depth-of-field simulation) to emphasise the 2.5D depth of the machinery.