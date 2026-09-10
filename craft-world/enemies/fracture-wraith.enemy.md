---
name: fracture-wraith
image:
  url: https://media.craftrpgs.com/containers/01a074ae-3136-72dc-a7f0-965bf56e0889/images/ee9ee32f-53b6-4420-899f-09bd5b0948b2
  generation:
    status: success
    startedAt: 2026-09-08T16:28:53.719Z
    finishedAt: 2026-09-08T16:29:04.147Z
    prompt: Translucent pale violet shattered-glass elongated diamond wraith
      silhouette drifting in dark void, glowing violet edges, trail of glass
      sparks, eerie purple point light, ruined lab background, dark sci-fi
      concept art, moody cinematic lighting, deep shadows with glowing accents,
      highly detailed game key art, no text, no UI, no watermark
$craft:
  referenceId: 01a074db-72c7-73e8-bf42-03ac7347f8fa
---

# Fracture Wraith — Environmental Hazard (Enemy Type)

> Designation: Aether Anomaly Class II — "Fracture Wraith"
> Origin: Crystallised gravity echo from the Core explosion

## Overview

Fracture Wraiths are not machines — they are autonomous gravity echoes: loops of broken physics that formed living patterns when the Core detonated. They resemble shattered glass silhouettes that drift through space along fixed gravity lines.

Touching a Wraith doesn't kill Kael — it triggers `triggerRandomGravityShift()`: gravity snaps to one of the four directions (DOWN (0,-9.81) F, UP (0,+9.81) R, LEFT (-9.81,0) Q, RIGHT (+9.81,0) E) and consumes the 0.8s gravity cooldown, leaving you tumbling mid-air or pinned to a ceiling. Wraiths attack your gravity vector, not your health — they exist to break your footing at the worst possible moment.

## Variants

| Variant | Behaviour |
|---|---|
| **Fracture Wraith** | Drifts along single axis |
| **Spiral Wraith** | Traces a slow circle — rotates gravity progressively |
| **Mirror Wraith** | Reflects gravity to the opposite of whatever Kael currently has |

## Behaviour

- Drifts slowly along a fixed gravity axis (always in the direction of the *current* gravity, ignoring player-set changes)
- Passes through walls and platforms — `isTriggerVolume` true, so it never participates in AABB minimum-penetration resolution
- Collision with player: `triggerRandomGravityShift()` — not death, but a puzzle disruptor
- Every variant is telegraphed: Mirror Wraith pulses brighter a beat before it echoes; Spiral Wraith leaves a violet arc along its orbit; base Wraith shimmers on shift contact

## Physics

```
Fracture Wraith (base):
  isTriggerVolume: true          -- AABB overlap only, no collision push
  halfExtents: (0.3, 0.6, 0.3)
  mass: 0.0                      -- not gravity-affected, kinematic drift
  driftSpeed: 1.5 units/sec
  onPlayerContact: randomGravityShift()

Spiral Wraith:
  isTriggerVolume: true
  halfExtents: (0.35, 0.35, 0.35)
  mass: 0.0
  orbitRadius: 3.0 units
  orbitSpeed: 0.5 rad/sec
  rotationProgress: 0.15 * time  -- progressively rotates gravity vector while orbiting

Mirror Wraith:
  isTriggerVolume: true
  halfExtents: (0.25, 0.7, 0.25)
  mass: 0.0
  driftSpeed: 2.2 units/sec      -- closes distance faster than base
  echoTick: 0.4s                 -- when player gravity is DOWN, echoes opposite (UP) for 2.0s
```

## Disabling & Counterplay

Wraiths cannot be damaged or destroyed — they are ambient field anomalies, not combatants. Counterplay is avoidance and timing:

- **Outpace them** — base drift is only 1.5 u/s and Kael is faster; keep moving and they can't touch you.
- **Cover does nothing** — Wraiths pass through walls, so hiding is pointless; break line-of-sight by distance, not geometry.
- **Read the patterns** — Spiral traces a fixed circle, Mirror only acts in 0.4s echo beats. Time your gravity switch to slip through the gap during the blend window (0.4s).
- **Exploit the cooldown** — a hit is survivable: the random shift burns your 0.8s gravity cooldown, so re-align immediately and use the forced lockout as a beat to reposition safely.
- **One blast per contact** — trigger fires once per pass; retreat and re-approach rather than lingering inside the volume and eating a second shift.

## Render (C++ Notes)

- Mesh: Thin elongated diamond (custom Mesh built from 8 vertices)
- **Colour:** Translucent pale violet — `(0.7, 0.5, 1.0)` with alpha `0.65`
- **Rendering order:** After opaque geometry, with `GL_BLEND` enabled
- Emits a faint continuous particle trail of shattered-glass sparks (vertex-fade over 1.2s)
- `PointLight: (0.5, 0.3, 0.9)`, radius 2 — gives eerie purple tint to surroundings
- Slow rotation on Z axis: `glm::rotate(model, time * 0.8f, {0,0,1})`