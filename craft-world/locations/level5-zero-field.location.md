---
name: level5-zero-field
image:
  url: https://media.craftrpgs.com/containers/01a074ae-3136-72dc-a7f0-965bf56e0889/images/faa1ef75-f350-4bb4-959e-96987c3ece35
  generation:
    status: success
    startedAt: 2026-09-08T16:28:53.726Z
    finishedAt: 2026-09-08T16:29:07.716Z
    prompt: Wide establishing shot of zero-gravity spherical chamber, floating rock
      islands and drifting violet crystals and shards suspended mid-air, cyan
      amber and red gravity field brackets, soft drifting particles, weightless
      calm, dark sci-fi concept art, moody cinematic lighting, highly detailed
      game key art, no text, no UI, no watermark
$craft:
  referenceId: 01a074db-72c8-723d-ba1a-9c800577b2e6
---

# Section E-5 — The Zero Field

> *Level 5: Zero-Gravity Chamber*
> *Theme: Scaled Gravity & Drift*

## Overview

The Aether Institute's primary null-gravity testing chamber — a vast spherical room designed to simulate microgravity. The Core explosion locked the entire chamber into a low-gravity loop: gravity is present but at **30% normal strength** (`g_eff = 0.3 * 9.81 = 2.943 m/s^2`), and there are **local Gravity Field zones** that have their own pull vectors.

This is the first level introducing **localised Gravity Fields**: physical zones where gravity is different from the rest of the level. It is also the first level where the *magnitude* of gravity is a variable — jumps are long and floaty, crates drift, and nothing quite falls the way the player expects.

## Visual Style

- **Palette:** Deep purple (#120820) with soft violet-white crystalline formations
- **Mood:** Vast. Dreamlike. Objects drift slowly. This is the most visually distinct level.
- **Depth layers:**
  - Back: the chamber's spherical wall, faintly mapped with the null-field warning stencil
  - Mid: floating islands and the three Gravity Field zones, outlined with coloured brackets (cyan UP, amber RIGHT, red DOWN)
  - Front: drifting crystal shards and slow dust, all moving on long ballistic arcs
- **Signature visual:** Floating crystal shards — debris that drifts slowly in the low gravity field. Provides environmental depth and serves as moving hazards.

## Drift Behaviour (low gravity)

- Global gravity is a **scaled vector**: `g_global = DOWN * (0.3 * G)`.
- Jump arcs are ~1.8x longer in airtime; the camera follows, giving a floaty, near-zero-g feel.
- Air drag model: dynamic bodies decelerate at `vel *= (1.0f - 0.02f * dt * 60.0f)` per frame, so crates keep drifting after leaving a Gravity Field instead of stopping instantly.
- Leaving a field does not snap gravity: gravity blends over 0.4s (cooldown 0.8s) toward the destination vector to keep the drift believable.

## Mechanics Introduced

### Gravity Fields
A Gravity Field is an AABB trigger volume with its own gravity vector (direction x strength). While inside the field:
- Player and dynamic objects experience the FIELD gravity, not global gravity
- Visual indicator: directional arrows rendered on field boundaries (↑↓←→)
- Exiting the field restores global gravity (with blend)

```
GravityField properties:
  AABB volume (trigger)
  gravityDirection: one of DOWN/UP/LEFT/RIGHT
  gravityStrength: float multiplier on G (can be < 1.0 for partial gravity)
  borderColor: (r,g,b) -- rendered as coloured outline
  g_effective = gravityDirection * (gravityStrength * G)
```

## Geometry

```
World bounds: X [0, 50], Y [0, 30]
Global gravity: DOWN at 30% strength (9.81 * 0.3 = 2.943 m/s^2)

Platforms (sparse — floating islands):
  - Island A: pos(0,2),  size(8,1)          -- start island
  - Island B: pos(14,8), size(6,1)          -- mid-level
  - Island C: pos(28,15), size(6,1)         -- high island
  - Island D: pos(40,5), size(8,1)          -- goal island

Gravity Fields (strength is x G, independent of the 0.3 global scale):
  - Field 1 [UP, 80%]:   AABB pos(10,0), size(8,12)    -- pulls UP inside zone
  - Field 2 [RIGHT, 60%]: AABB pos(22,10), size(10,10) -- pulls RIGHT
  - Field 3 [DOWN, 150%]: AABB pos(36,20), size(12,10) -- strong DOWN (crushing field)

Objects:
  - Floating Crate A: pos(12,4)    -- drifts into Field 1, player must redirect it
  - Pressure Plate: pos(46,6)      -- on Island D, Crate A must reach it
  - ARIA node (fragmented): pos(0,3) -- partial signal, first contact since D-4
  - Goal: pos(48,6)

Drifting Crystal Hazards (moving, collision = death):
  - Crystal 1: drifts along Y axis at X=20, speed 1.2
  - Crystal 2: drifts along X axis at Y=15, speed 0.8
  - Crystal 3: slow circle at center pos(25,15), radius 6

Enemies:
  - Sentinel-Phase: drifts through Field 2 -- must be lured into Field 3 to destroy
  
Checkpoint: Island B pos(16,9)
```

## Narrative

ARIA returns briefly — first contact since the Relay Chambers: *"Zero field... I remember configuring this room. It was beautiful. A perfect sphere of nothing. We were so proud."*

Kael finds the authorization log for the Core sequence. It requires two signatures. One is his. The other is ARIA's.

*ARIA:* A long pause. *"I should tell you something."*

## Physics Demonstration (CGG Note)

> **Scaled gravity vectors** — Gravity is stored and applied as a *scaled vector*, not a constant: `g_effective = direction * (strength * G)`. The world scale sits at `0.3f`; each GravityField substitutes its own `(direction, strength)` pair. This demonstrates affine scaling of a physics vector — magnitude and direction are independent knobs, which is what lets a "zero-G" room still contain local pull fields.

> **Drift integration** — Dynamic bodies use explicit Euler with drag: `vel += g_effective * dt; vel *= (1.0f - 0.02f * dt * 60.0f); pos += vel * dt;`. The constant low magnitude plus drag is what turns platforming into long, forgiving drifts.

> **Circle Drawing (CGG concept link)** — Gravity Field boundaries render as dashed arcs using a Midpoint Circle algorithm adapter (concept from `midpoint_circle.cpp`) to draw rounded corner brackets around each field.