---
name: level4-relay-chambers
image:
  url: https://media.craftrpgs.com/containers/01a074ae-3136-72dc-a7f0-965bf56e0889/images/197929ba-fbce-412e-99e6-6e8131c58ebf
  generation:
    status: success
    startedAt: 2026-09-08T16:28:53.737Z
    finishedAt: 2026-09-08T16:29:03.769Z
    prompt: Wide establishing shot of relay chamber sequence, dark signal conduit
      halls with glowing cyan pulse lines running between valves and tall blue
      vault doors, amber pressure plates, signal routing lights, fading hologram
      static, dark sci-fi concept art, moody cinematic lighting, highly detailed
      game key art, no text, no UI, no watermark
$craft:
  referenceId: 01a074db-72c7-73e8-bf42-15cb284736c8
---

# Section D-4 — The Relay Chambers

> *Level 4: Multi-Gravitational Chain Reaction*
> *Theme: Relay Signal & Door Chains*

## Overview

The Institute's signal relay network — a series of interconnected chambers linked by gravity-powered conduits. Each chamber has its own gravity state, and events in one chamber propagate to the next. The player must trigger switches in the correct order while navigating shifting gravity across multiple rooms.

The level is one circuit: every door here is a **signal gate**. A door opens only when its upstream conduit valve reports ACTIVE, and a valve only activates when its chamber's gravity puzzle is resolved. The whole level runs A → B → C → the goal door, and the player can always *see* the energy pulses travelling the conduits — the answer to every puzzle is written in the wiring.

ARIA's signal cuts out completely here. Kael is alone — no voice, no HUD guidance. The soundscape drops to pure diegetic hums and conduit pulses.

## Visual Style

- **Palette:** Cold midnight blue (#050A14) with intermittent electric-white conduit pulses; valve activations bloom cyan (#6FD3FF) for 0.8s.
- **Mood:** Silent. Oppressive. The first level where Kael realises he has no guidance.
- **Depth layers:**
  - Back: inert relay towers and machinery silhouettes, barely lit against the void
  - Mid: chamber walls and the gravity conduits — glass tubes that visibly carry energy between chambers
  - Front: conduit pulse heads, EMP sparks, dust motes drifting in each chamber's gravity direction
- **Signature visual:** Gravity conduits — glowing tubes that carry energy between chambers. When a switch activates, a bright energy pulse travels the tube valve-to-door in 0.8s: cause and effect, shown physically.

## Relay & Signal Mechanics

- **Conduit Valve:** one-time switch; sets its node to ACTIVE and fires a pulse down its tube.
- **Signal pulse:** travels the conduit spline in 0.8s (`pulseTime`), rendered as a bright leading head; the pulse's arrival is the player's feedback that a lock released.
- **Door lock (AND gate):** door opens only while its required upstream nodes are ACTIVE. Indicator lamps beside each door show relay state — a green lamp per satisfied input.
- **Door state:** `doorOpen = (relayState & doorMask) == doorMask`. Doors are static colliders that toggle off when the gate opens; no door re-locks once opened.

## Geometry (Three connected chambers)

```
Chamber 1 — "The Initiator"
  World region: X [0, 16], Y [0, 10]
  Gravity start: DOWN
  
  Objects:
    - Gravity Switch [UP]: pos(8,1)   -- required to reach conduit valve
    - Conduit Valve A: pos(8,9)       -- triggers Chamber 2 door
    - ARIA node (silent): pos(2,1)    -- dead comm terminal, visual only

Chamber 2 — "The Intermediary"
  World region: X [16, 32], Y [0, 18]
  Gravity start: LEFT
  Enters with gravity already LEFT — disorienting by design
  
  Platforms:
    - Left wall floor: pos(16,0), size(1,18)    -- "ground" under LEFT gravity
    - Floating platforms: pos(16,5) size(3,1), pos(16,11) size(3,1)
    
  Objects:
    - Crate: pos(16,2) -- must be pushed onto plate on right wall
    - Pressure Plate: pos(31,9)
    - Gravity Switch [RIGHT]: pos(20,10)
    - Conduit Valve B: pos(31,16)      -- triggers Chamber 3 door

Chamber 3 — "The Payoff"
  World region: X [32, 48], Y [0, 14]
  Gravity start: DOWN (reset at chamber entry)
  
  Objects:
    - Sentinel-Heavy: pos(40,1)
    - Gravity Switch [UP]: pos(35,1)
    - Gravity Switch [LEFT]: pos(48,13)
    - Conduit Valve C: pos(33,13)   -- final valve
    - Goal: pos(47,1)               -- door opens only when A+B+C valves activated

Enemies:
  - Fracture Wraith: drifts vertical path X=24, Y[2..16] -- disrupts gravity mid-puzzle
  
Checkpoints: Chamber 2 entry pos(17,1), Chamber 3 entry pos(33,1)

Signal routing:
  Valve A(8,9)    --pulse 0.8s--> Chamber 2 door lock (needs A)
  Valve B(31,16)  --pulse 0.8s--> Chamber 3 door lock (needs B)
  Goal door       : needs A AND B AND C (all three valves ACTIVE)
```

## Narrative

Mid-level Kael finds another log terminal. *His* voice, pre-catastrophe: *"The relay network should be able to redirect the Core's output safely. Trust the calculations."*

Kael says aloud: *"I trusted the calculations."*

**ARIA signal-loss beat:** Entering Chamber 1, ARIA's comm link degrades mid-sentence and dies at the first valve: *"Kael, the array is re-routing my— …los— …ing you— …"* Her HUD marker goes grey; no voice guidance for the rest of the level. The silence is the design: here doors answer only to physical signals, not to her voice. (Comm link re-establishes at the top of Level 5.)

## Physics Demonstration (CGG Note)

> **Door state as a bitmask** — Each valve node is one bit of a `uint8_t relayState`. The door-open test is a single bitwise AND: `(relayState & doorMask) == doorMask`. The "chain" is spatial, not sequential — so the player's route is readable at a glance and the code is a two-line gate test per door.

> **Conduit pulse as a quadratic Bézier** — The pulse is a bright quad travelling the conduit's control points, evaluated with `B(t) = (1-t)^2 P0 + 2(1-t)t P1 + t^2 P2` where `t = pulseTime / 0.8s`. One evaluation per active pulse per frame: the head's arrival at the door is the visible "signal" that unlocked the gate.

> **Composite Transformations** — Each chamber has independent coordinate axes based on gravity direction. An object's "up" is computed as `-gravityVector.normalized()`. All transforms compose: `World = T_position * R_gravity * R_tilt * S_scale`. This is the explicit CG concept of composite matrix multiplication (`M = M1 * M2 * M3`).