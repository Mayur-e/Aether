# AETHER — ASSET REQUESTS
> Maintained per AETHER_MASTER_ALIGNMENT_REBUILD.md §47

---

## REQUEST-001 — KAEL (Player Character)

```
ASSET REQUEST — PHASE 2 / ALL LEVELS

Asset:
Kael — Player Character (3D)

Required for:
Player representation in all 7 levels.

Purpose:
Kael is the protagonist and the physical object the player controls.
The current prototype uses a plain 0.8×1.8 unit box as a placeholder.
Phase 2 requires this placeholder to be replaced with a real character.

Type:
Character — Skeletal Mesh (3D)

Required visual style:
- Young male researcher, lean/wiry build
- Short dark windswept hair
- Torn research coat over graphene underlayer
- Luminous Aether burn mark on left arm (emissive material)
- Dark cinematic science-fantasy aesthetic
- Stylized low-poly or cinematic 3D (NOT cartoon, NOT anime, NOT photorealistic)
- Must visually read as a non-combat researcher, not a soldier

Reference:
AETHER_MASTER_ALIGNMENT_REBUILD.md §4 — Kael character requirements

Required file format:
PREFERRED: GLB or GLTF (includes mesh + materials + textures + animations in one file)
ACCEPTABLE: FBX
OPTIONAL SOURCE: BLEND (Blender source file)

Required texture formats:
PNG preferred

Required texture maps:
- Base Color
- Normal
- Roughness
- Metallic (where applicable)
- Emission (for Aether burn mark on left arm)

Required mesh parts:
- Head
- Hair
- Torso (research coat)
- Arms
- Legs
- Aether burn mark / emissive detail on left arm

Required animations (ideally embedded in GLB/FBX):
- Idle
- Run / Walk
- Jump (start)
- Fall / In-air
- Land
- Interact
- Death / Reset

Approximate scale:
Character height ~1.8 units (matching current physics body half.y = 0.9)

Pivot / origin requirement:
Bottom-center of feet

Material requirements:
- Coat/body: opaque PBR
- Burn mark: emissive

Performance target:
Low-poly to medium-poly, max ~5000 triangles recommended

Where it will be placed:
All 7 levels — player object

Do we already have it?
NO

ACTION REQUIRED FROM USER:
Please provide ONE of the following:
  A) A Kael 3D character file (GLB / GLTF / FBX) matching the above spec
  B) A Blender (.blend) source file to export from
  C) Explicit written approval to use a documented low-poly placeholder
     for this development phase (placeholder will be recorded as temporary
     and flagged for replacement before final release)
```

---

## REQUEST-002 — ARIA (Holographic Projector Node)

```
ASSET REQUEST — PHASE 2 / LEVELS 1-7

Asset:
ARIA — Holographic Projector Node (3D)

Required for:
Tutorial guidance, narrative, warnings, environmental information.

Purpose:
ARIA appears through fixed holographic projector nodes placed in levels.
She is NOT the player. She provides tutorial text and story beats.

Type:
Environment Prop + Character Visual

Required visual style:
- Cyan hologram, translucent/emissive
- Futuristic projection aesthetic
- Two parts: (1) physical projector device on floor/wall,
  (2) holographic "figure" or "form" that appears above it
- States: IDLE (slow pulse), SPEAKING (brighter), FRAGMENTED (glitchy),
  OFFLINE (dark)

Required file format:
PREFERRED: GLB / GLTF
ACCEPTABLE: FBX

Required texture maps:
- Base Color
- Emission (primary visual element)
- Alpha (for translucency)

Performance target:
Low-poly, max ~2000 triangles

Where it will be placed:
Level 1 (The Arrival Hall) and subsequent levels

Do we already have it?
NO — ARIA has zero in-game presence currently.

ACTION REQUIRED FROM USER:
Same three options as REQUEST-001:
  A) Provide ARIA asset file
  B) Provide Blender source
  C) Approve use of a temporary emissive-cyan box placeholder
     (flagged for replacement)
```

---

## REQUEST-003 — SENTINEL (Enemy)

```
ASSET REQUEST — PHASE 2 / LEVELS 2+

Asset:
Sentinel — Ground Patrol Enemy (3D)

Required for:
Levels 2, 3, 4, 6, 7.

Purpose:
Sentinel is a gravity/puzzle hazard that patrols a defined path.
Touching it resets the level. It is NOT a conventional shooter enemy.

Variants required (can share base mesh + color variation):
- Sentinel-Standard  (Levels 2, 3, 7)
- Sentinel-Heavy     (Levels 4, 7)
- Sentinel-Swarm     (Level 6)
- Sentinel-Phase     (Level 5)

Required visual style:
- Mechanical/robotic gravity enforcement unit
- Dark with energy accent colors (cyan/amber)
- AETHER facility aesthetic
- Must NOT look like a human soldier or fantasy monster

Required file format:
PREFERRED: GLB / GLTF
ACCEPTABLE: FBX

Performance target:
Low-poly, max ~3000 triangles per variant

Do we already have it?
NO — currently a generic cube.

ACTION REQUIRED FROM USER:
  A) Provide Sentinel asset file
  B) Provide Blender source
  C) Approve temporary placeholder (colored cube with emissive material)
```

---

## REQUEST-004 — FRACTURE WRAITH (Enemy)

```
ASSET REQUEST — PHASE 2 / LEVELS 4+

Asset:
Fracture Wraith — Gravity Anomaly Entity (3D)

Required for:
Levels 4, 7.

Purpose:
Wraith is a flying enemy that pursues the player.
It ignores gravity. It is translucent and violet.
Touching it resets the level.

Required visual style:
- Translucent violet energy entity
- Gravity-anomaly / distortion aesthetic
- Fracture / shard visual motif
- Non-conventional enemy appearance

Required file format:
PREFERRED: GLB / GLTF
ACCEPTABLE: FBX

Required texture maps:
- Emission (violet)
- Alpha (translucency)

Performance target:
Low-poly, max ~2000 triangles

Do we already have it?
NO — currently a generic cube.

ACTION REQUIRED FROM USER:
  A) Provide Wraith asset file
  B) Provide Blender source
  C) Approve temporary placeholder (violet semi-transparent cube)
```

---

## Status Summary

| ID | Asset | Blocking Phase | Status |
|---|---|---|---|
| REQUEST-001 | Kael (player) | Phase 2 | WAITING |
| REQUEST-002 | ARIA projector | Phase 2 | WAITING |
| REQUEST-003 | Sentinel enemy | Phase 2 / L2+ | WAITING |
| REQUEST-004 | Fracture Wraith | Phase 2 / L4+ | WAITING |
