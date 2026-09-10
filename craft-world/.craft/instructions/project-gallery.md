# Project Gallery

The gallery is the curated strip of art on the project's cover page, under
the heading "Glimpses": `settings.gallery` (the array) and
`settings.galleryLayout` (the shape) in `.craft/project.json`. Edit the
file in place, then plain `craft push` — no flag needed. See the
configuring-your-project topic for the settings file's own rules.

Curating it is the last step of a finished world, not a chore: pick from the
art you have already generated rather than generating for the gallery.

## Item shape

An array of up to 48 showcase items. Each item:

```json
[
  { "kind": "image",
    "url": "https://media.craft.example/projects/abc/hall-of-echoes.webp",
    "caption": "The Hall of Echoes, an hour before the vote",
    "focalPoint": { "x": 0.42, "y": 0.31 } },
  { "kind": "image",
    "url": "https://media.craft.example/projects/abc/vellin.webp",
    "caption": "Vellin Ashgrave, first of the exiles" },
  { "kind": "video",
    "url": "https://media.craft.example/projects/abc/storm.mp4",
    "caption": "The season turns",
    "animation": {
      "kind": "generated_video",
      "posterUrl": "https://media.craft.example/projects/abc/storm-poster.webp",
      "mp4Url": "https://media.craft.example/projects/abc/storm.mp4"
    } }
]
```

- `kind` — `"image"` | `"gif"` | `"video"`. Required.
- `url` — required, must be a real Craft-hosted media URL (see below).
- `caption` — optional, ≤280 characters. It is display copy, not alt text:
  one line that makes a viewer want the world.
- `focalPoint` — optional `{x, y}`, each 0–1. Layouts crop items to their
  slot's aspect; the focal point is what stays in frame. Set it when the
  subject is off-center.
- `animation` — optional, only for moving items:
  `{kind: "animated_gif"|"generated_video", posterUrl, mp4Url, webmUrl?}`.

**The item schema is strict.** An unknown key does not get ignored — the whole
write is rejected. In particular a gallery item is NOT a file's `image`
object: `generation`, `variations`, and `backgroundRemovedUrl` are
lifecycle fields the app and the image pipeline own, and pasting an image
object straight in fails validation. Take `url` (and `focalPoint` /
`animation` if present) out of it and leave the rest where it is.

## Where URLs come from

Every URL must be one Craft already hosts. There are exactly three honest
sources: an image generation result, a media upload result, or the `url`
already stored on a file's `image` field. **Never write a URL you have not
seen in data** — an invented R2 path saves fine and renders as a broken tile
on the project's front page, which is the one page strangers judge the world
by. If you want art that does not exist yet, generate it first.

## Ordering and layout

Order is meaning: items fill the layout's slots top-left first, so item 1 is
the hero. `galleryLayout` picks the shape and how many slots exist —

- `"mosaic"` (the default) — asymmetric magazine spread, 6 slots.
- `"featured"` — one large hero plus four stacked portraits, 5 slots.
- `"grid"` — uniform 4-across, 12 slots.
- `"filmstrip"` — horizontal scroller, unbounded.

Items past a grid layout's slot count are not shown; they collapse into a
"+N more" badge on the last slot. So match the count to the layout: three or
four strong captioned glimpses beat twelve competent ones, and a gallery of
near-identical portraits reads worse than one landscape, one face, one
artifact, one scene.

## Getting the URLs, from here

`craft image generate --json` and `craft image upload --json` each return
a complete `image` object — take its `url` (plus `focalPoint` /
`animation` when present). Otherwise read `image.url` straight out of a
workspace file you have already illustrated; `.craft/index.json` lists what
exists if you need to go looking.
