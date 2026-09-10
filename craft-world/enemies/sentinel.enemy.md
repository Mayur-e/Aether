---
name: sentinel
image:
  url: https://media.craftrpgs.com/containers/01a074ae-3136-72dc-a7f0-965bf56e0889/images/215a315b-105d-4c56-8f6e-5c5f569ab2ed
  generation:
    status: success
    startedAt: 2026-09-08T16:28:53.693Z
    finishedAt: 2026-09-08T16:28:57.342Z
    prompt: Floating charcoal-grey chamfered cube security drone with single glowing
      red eye light, red emissive aggro glow, hovering in dark ruined research
      corridor, faint red shockwave ring, dark sci-fi concept art, moody
      cinematic lighting, deep shadows with glowing accents, highly detailed
      game key art, no text, no UI, no watermark
$craft:
  referenceId: 01a074db-72c7-73e8-bf42-040fa3536119
---

# Sentinel — Gravity Drone (Enemy Type)

> Designation: **SEC-GD-7 "Sentinel"**
> Status: Autonomous. Hostile. Unaware of facility failure.

## Overview

Sentinels are decommissioned security drones that patrolled the Aether Institute's research sections. The Core catastrophe erased their shutdown protocols. They now execute their last known directive — *maintain gravity alignment in all sections* — which means they actively try to reverse any gravity change the player makes.

They don't attack Kael directly. They attack his gravity vector.

## Variants

| Variant | Colour | Behaviour |
|---|---|---|
| **Sentinel-Standard** | Charcoal grey | Patrols a fixed route, resets gravity to DOWN when Kael switches |
| **Sentinel-Heavy** | Dark red | Stationary. Resets gravity AND locks it for 3 seconds |
| **Sentinel-Swarm** | Silver | 3 small drones in formation. Must all be grounded (stood on) to disable |
| **Sentinel-Phase** | Translucent blue | Passes through walls. Cannot be stood on. Must be lured through a Gravity Field |

## Behaviour

- Drone state machine: `PATROL → ALERT (0.5s) → RESET → COOLDOWN → PATROL`
- PATROL: Standard and Swarm follow a fixed route; Heavy and Phase hold station. All rotate on Y at 45 deg/sec while patrolling.
- On detecting a gravity switch (gravity blend 0.4s, cooldown 0.8s): enters ALERT — eye light flares, emissive spikes — then RESET fires, fighting the player's vector back to baseline `(0, -9.81)` after `gravityResetDelay`.
- Phase drone phases through wall AABBs while ethereal and never offers a foothold; it uses non-solid overlap checks only.
- When disabled, drops to LOW POWER: dark, unlit, drifts to a rest pose.

## Physics Properties

```
Sentinel-Standard:
  halfExtents: (0.4, 0.4, 0.4)     -- cubic AABB
  mass: 0.0                          -- static (kinematic) — not affected by gravity
  moveSpeed: 2.5
  patrolRadius: 8.0 units
  gravityResetDelay: 1.5 seconds     -- how long after player switches before sentinel resets
  resetVector: (0, -9.81)            -- baseline DOWN

Sentinel-Heavy:
  halfExtents: (0.6, 0.6, 0.6)
  mass: 0.0
  moveSpeed: 0.0                     -- holds station; no patrol route
  gravityLockDuration: 3.0 seconds
  lockCooldown: 6.0 seconds

Sentinel-Swarm (each drone):
  halfExtents: (0.2, 0.2, 0.2)
  mass: 1.0                          -- IS affected by gravity (can be knocked off a surface)
  moveSpeed: 2.0
  formationSpacing: 1.2 units
  disableCondition: grounded == false for > 1.0s (knocked off)

Sentinel-Phase:
  halfExtents: (0.3, 0.3, 0.3)
  mass: 0.0                          -- kinematic
  moveSpeed: 3.0                     -- fastest variant; closes on Kael's position
  phaseCycle: 2.0s solid / 1.0s ethereal  -- ethereal skips wall AABB overlap entirely
  disableCondition: inside reversed-Gravity Field for 1.5s → LOW POWER
```

## Disabling Sentinels

Sentinels cannot be permanently destroyed. They can be:
- **Grounded** — Kael stands on top: Sentinel powers down temporarily (8 seconds)
- **Gravity-trapped** — flipping gravity while Sentinel is mid-air causes it to slam into a wall and stun for 5 seconds
- **Lured into Gravity Fields** — inside a reversed-gravity zone, Sentinels behave erratically and reset to LOW POWER mode

Per-variant disable rules:

| Variant | How to disable |
|---|---|
| **Standard** | Stand on it → powers down for 8s |
| **Heavy** | Cannot be grounded (armoured). Gravity-trapped only — slam it during its 6s lock-cooldown window |
| **Swarm** | Ground all 3 drones simultaneously, or flip gravity to knock them off a surface (each must stay grounded-false > 1.0s) |
| **Phase** | Cannot be stood on at all. Lure it through a Gravity Field for 1.5s → LOW POWER (10s) |

## Render (C++ Notes)

- Mesh: Cube with chamfered visual (achieved via scale `(0.85, 0.85, 0.85)` on inner cube + outer wire cube)
- **Standard colour:** `(0.25, 0.25, 0.28)` with red eye light `PointLight: (1.0, 0.1, 0.1)` radius 1.5
- **Aggro state colour:** `(0.7, 0.1, 0.1)` — emissive red when resetting gravity
- **Disabled colour:** `(0.1, 0.1, 0.12)` — dark, no light
- Rotation animation: slowly rotates Y axis `45 deg/sec` on patrol
- On gravity-reset trigger: emit red shockwave particle ring from drone position