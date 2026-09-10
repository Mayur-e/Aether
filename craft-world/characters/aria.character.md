---
name: aria
image:
  url: https://media.craftrpgs.com/containers/01a074ae-3136-72dc-a7f0-965bf56e0889/images/758dbe01-799b-41ee-a9cc-06ca42361a69
  generation:
    status: success
    startedAt: 2026-09-08T16:28:53.702Z
    finishedAt: 2026-09-08T16:29:00.676Z
    prompt: Small oval holographic projector node mounted on a dark institute wall
      projecting a soft cyan wireframe floating human silhouette, glowing cyan
      light radius, particles of cyan sparks, dark corridor background, dark
      sci-fi concept art, moody cinematic lighting, deep shadows with glowing
      accents, highly detailed game key art, no text, no UI, no watermark
$craft:
  referenceId: 01a074db-72c7-73e8-bf41-f89a1a15ff0e
---

# ARIA — Aether Residual Intelligence Array

> *"Systems nominal. Gravity: undefined. Please do not panic."*

## Overview

ARIA is the Aether Institute's still-functional AI assistant — a voice in Kael's earpiece and the glowing holographic projector nodes scattered through the facility. She was designed to guide researchers through the Institute's experimental zones. Post-catastrophe, she persists in fragmented form: calm, precise, and occasionally helpful in ways she doesn't fully understand.

ARIA is the tutorial voice. She explains mechanics, warns of hazards, and delivers dry expository dialogue.

## Physical Description

- **Manifestation:** Small oval holographic projector nodes mounted on walls throughout the facility. Projects a soft cyan wireframe of a floating human silhouette when "speaking."
- **Voice:** Measured, clinical, subtly worried beneath the precision.
- **Colour:** Cyan (#00D4FF) with soft pulse animation. Emits point light radius ~2m (see Physics Properties — 3.0 in gameplay units).

## Role in Gameplay

| Scene | ARIA's Function |
|---|---|
| Level 1 | Introduces movement, jumping, gravity flip |
| Level 2 | Warns about wall gravity zones |
| Level 3 | Explains crate + pressure plate puzzle logic |
| Level 4 | Goes quiet — signal loss deeper in the facility |
| Level 5–7 | Returns briefly with increasingly fragmented transmissions |
| AETHER Core | Final transmission before shutdown sequence |

## Physics Properties (C++ Implementation Reference)

ARIA nodes are **static hologram projector triggers** — they never move and never block the player, but they wake on overlap to fire tutorial VO and prompts.

```
mass: 0.0                        -- Static; ignored by gravity sweeps, never moved
kinematic: true                  -- Placed in world at build time
halfExtents: (0.25, 0.25, 0.25)  -- Small AABB around the projector node
status: trigger                  -- Overlap-detection only; no collision response
wakeRadius: 3.0                  -- Player AABB overlap within radius -> SPEAKING state
pointLight:
  color: (0.1, 0.7, 1.0)        -- Cyan
  radius: 3.0
emissive: (0.0, 0.83, 1.0)       -- #00D4FF, emissiveStrength 2.5
pulse: amplitude 0.08, rate 2.0 Hz   -- Sine drives emissive strength + scale
```

## States

- `IDLE` — dormant projector at low power (20% brightness), no voice line
- `PULSING` — within `wakeRadius`; light quickens, waiting for interaction/overlap trigger
- `SPEAKING` — full hologram projection, VO line playing, point light at full radius
- `FRAGMENTED` — corrupted transmission: partial wireframe, garbled audio, flickering light (Levels 4–7)
- `OFFLINE` — dead node after shutdown: no light, no voice; used at AETHER Core finale

## Key Dialogue Examples

> **Level 1 Entry:**
> "Dr. Kael. Welcome back to Section A-1. I should note — the laws of physics in this area are currently suggestions. I recommend treating them as such."

> **On first gravity flip:**
> "Aether field aligned to new vector. Please note: the floor is now the ceiling. This is, technically, fine."

> **Level 4 signal loss:**
> "Ka... signal degr—... Section D is non-nominal. I cannot... [static]"

## Narrative Arc

ARIA begins as the reliable tutorial voice — methodical, reassuring, in control. As Kael pushes deeper, her coverage fractures: nodes offline, transmissions clipped and repeated, her calm breaking into loops of the original catastrophe. By the AETHER Core she is barely coherent, but her final fragment resolves into one clear instruction: the shutdown terminal Kael was always looking for. In the last scene she goes `OFFLINE` — deliberately, at minimum power, delivered with dry relief.

## Render (C++ Notes)

- ARIA nodes are small sphere meshes with strong emissive blue-cyan tint
- `emissiveColor: (0.0, 0.83, 1.0)`, `emissiveStrength: 2.5`
- Associated `PointLight`: `color: (0.1, 0.7, 1.0)`, radius ~3 units
- Transform is static: `model = glm::translate(glm::mat4(1.0f), nodePosition);` — nodes never rotate or translate at runtime
- Animate with slow sine-wave pulse: `scale = 1.0 + 0.08 * sin(time * 2.0)`
- When "speaking": emit particle burst of cyan sparks from node position
- `FRAGMENTED` state: modulate emissive strength with high-frequency noise instead of the clean sine pulse

---
