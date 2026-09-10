# Authoring Maps

A world without a map is a list of place names. In the app, the project's
ROOT MAP is the world's front door: pins open locations, locations carry
their own maps (hierarchy), regions outline territories, and the
background image IS the map. Maps live on LOCATION files (any file whose
type has the "location" designation) as app-managed metadata — never in
the working tree — so they are authored with a dedicated command pair:

```
craft map get "Places/The Verdant Reach.location.json"
craft map set "Places/The Verdant Reach.location.json" --file map.json
```

The target file must be PUSHED first (the command is id-addressed).
`set` replaces the whole map — get, edit, set. The CLI advances the sync
ledger past the save automatically; no pull needed.

Map assembly needs the `craft` CLI (or the user editing the map in the
app). Working without the CLI: finish the location files and any
background images you can, then tell the user which maps remain to
assemble in the app — do not hand-author map metadata in `$craft`
blocks to work around it.

## The authoring JSON

```json
{
  "aspectRatio": { "width": 4, "height": 3 },
  "background": { "url": "https://…", "offset": { "x": 0, "y": 0 }, "scale": 1 },
  "scale": { "widthMeasurement": 60, "unit": "mi" },
  "areas": [
    {
      "label": "The Ashfall Wastes",
      "link": "Places/Ashfall Wastes.location.json",
      "vertices": [ { "x": 0.55, "y": 0.2 }, { "x": 0.9, "y": 0.25 }, { "x": 0.8, "y": 0.6 } ],
      "style": { "fillColor": "#b45309", "fillOpacity": 0.2, "strokeColor": "#b45309", "strokeWidth": 2 }
    }
  ],
  "points": [
    {
      "kind": "pin",
      "label": "Last Chime",
      "link": "Places/Last Chime.location.json",
      "position": { "x": 0.42, "y": 0.55 },
      "style": { "color": "#dc2626", "icon": null }
    }
  ]
}
```

- **Coordinates are normalized 0–1**, (0,0) top-left. Place elements
  where they sit ON THE BACKGROUND IMAGE — sanity-check a few against the
  image before saving many.
- **`link`** accepts a workspace path, a `slug.type` token, or a
  referenceId, and becomes the element's stored file link (the pin/area
  opens that file in the app, and the GM can navigate it). Omit `link`
  for pure annotations — then `label` is the visible text.
- **`aspectRatio` is required** and should match the background image
  (a default `craft image generate` is landscape-4-3 → `{4, 3}`).
- Element `id`s, point styles, and null-able fields (scale, grid,
  background, boundaryPolygon) are defaulted by the CLI.
- `grid`: add `{ "mode": "square", "cellSize": { "value": 5, "unit": "ft" } }`
  (or hex) ONLY on tactical/battle maps; region and world maps use
  `"scale"` instead. `points[].kind`: `"pin"` marks places,
  `"token"` marks characters/monsters — tokens are play-state, so
  worldbuilding maps mostly want pins.

## Hierarchy and the root map

- Pins that `link` to another location create the hierarchy: the child
  location's OWN map is the next level down. A child that shouldn't have
  its own map defers with the minimal payload
  `craft map set <child> --file <f>` where f is just
  `{"useParentMap": true}` — everything else is defaulted.
- Set the project's root map: put the world-map location file's
  `referenceId` (from `.craft/index.json`) into
  `settings.rootMapFileReferenceId` in `.craft/project.json` and
  `craft push`. A project without a root map has no map tab content.

## Backgrounds — prompt the layout, then pin YOUR layout

The background image is 90% of a map's quality, and its URL must be
Craft-hosted: `craft image generate` it (or `craft image upload`).
Prompt for a MAP, not a scene: "hand-drawn fantasy region map, top-down,
inked coastlines, labeled terrain, parchment texture" beats a landscape
painting.

**Never draw a generic map and guess where things landed.** Two-step
discipline:

1. **Dictate the composition in the prompt** ("walled trade-post at
   lower left; river crossing at center; grey cracked wasteland upper
   right; sunken ruin in a lake at the right edge…"), then place pins
   at the positions YOU dictated. Keep it to ~4–7 placed features —
   models follow short layout lists far better than long ones.
2. **Then verify against the real image**: download the returned URL
   (curl) and LOOK at it if your tooling reads images, and correct any
   pin whose feature drifted. Generation follows layout prompts
   approximately, not exactly — the verify pass is what makes pins land
   on their art.

Perspective by map kind: region/world maps — "top-down cartography".
Battle/dungeon maps — prompt "one contiguous top-down floor plan,
straight-down view, rooms connected by corridors, consistent scale";
without that you get a collage of pretty room vignettes that reads as a
diagram, not a grid you can move tokens across.

Type-level style instructions apply via `--for`; consider whether the
project's general art style suits cartography — if not, pass a
map-specific prompt and rely on `--no-style`, or give the location
type its own imageStyleInstructions.

## What a finished map set looks like

1. A root/world map: background image, major regions as linked areas,
   every significant location pinned and linked, `scale` set.
2. Each pinned location either has its own map (settlement/dungeon level,
   same bar) or `useParentMap: true`.
3. No orphan locations: if a location file matters to the story, it
   appears on some map.
4. Labels double-checked against the background art (a pin labeled
   "harbor" in the mountains reads as broken).
5. Tactical grids only where combat happens.
