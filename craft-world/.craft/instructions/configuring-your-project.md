# Configuring Your Project

Project settings are workspace files, not app-only state. Two surfaces:

## `.craft/project.json` — project-level settings

Edit the file, then plain `craft push` (no flag needed). The push is
guarded by the settings hash: if settings changed remotely since your last
sync you get `settings_hash_mismatch` — `craft pull`, merge, retry.
(No CLI? Settings edits also land through platform import: the app applies
them by default, with a per-setting veto in the preview.)

Top level: `name`, `description`, `tags`. Everything else lives under
`settings`. The ones agents most often should set:

- **`imageStyleInstructions`** — the project's ART DIRECTION, applied to
  every image generation (yours via `craft image generate`, the user's in
  the app, the GM's during play). When you establish a world's visual
  style, STORE IT HERE — do not hand-carry a style phrase into each
  prompt. One or two sentences of medium, palette, mood, rendering style:
  "Muted watercolor with ink outlines; cold light, weathered surfaces;
  no photorealism."
- **`defaultImageModel`** — model used when a generation doesn't name
  one (including `craft image generate` without `--model`).
- **`autoImageGeneration`** — whether the app auto-illustrates newly
  created files. Leave as the user set it unless asked.
- **`gm`** (`name`, `personality`, `voiceId`) — the Game Master
  persona used in play. Setting `personality` to match the world's tone
  is part of finishing a game system. Run `craft voices` to see the current
  voice ids and traits before setting `voiceId`; use `--json` when an
  agent needs to select or filter the catalog programmatically.
- **`headline`, `headlineLabel`, `body`, `authorsNotes`,
  `gameSystemName`** — the project page's presentation copy.
- **`gallery` + `galleryLayout`** — the cover page's "Glimpses"
  section: an array of showcase items (Craft-hosted URLs), laid out as
  `"mosaic"`, `"featured"`, `"grid"`, or `"filmstrip"`. Three or
  four captioned glimpses of your best generated art make the cover page
  feel alive. Item shape and layout slot counts: the project-gallery topic.
- **`playerHandbook`** / **`builderManual`** — the Player's Handbook
  (how to PLAY this system, read by players at the table) and the Builder's
  Manual (how to BUILD on it, read by creators adopting it). Both share one
  chaptered shape: `{"version": 1, "chapters": [{"id": "combat", "title":
  "Combat", "content": "…markdown…"}]}`. Both are real books; long-form
  rules prose belongs here, NOT in `body` (that's the project page's
  short pitch copy). What to put in each, plus caps, file mentions and
  images: the writing-handbooks topic.
- **`preludeFlow`** — the cinematic opening played in the game lobby,
  a flow manifest you can author: see the authoring-flows topic. The CLI
  validates it on push.
- **`rootMapFileReferenceId`** — the location file whose map is the
  project's ROOT map (the map tab's front door). Use the file's
  `referenceId` from `.craft/index.json`; see the authoring-maps
  topic.

**The file is pushed WHOLE, not as a patch — removing a key clears that
setting server-side.** So EDIT the file in place. Never regenerate
`project.json` from the keys documented here: the two biggest values in
it (`playerHandbook`, `builderManual`) are books someone wrote by
hand, and a "clean" rewrite that omits them silently deletes both. Same
for `preludeFlow`. Read the file, change the keys you were asked to
change, write it back. Don't touch keys you don't understand — `theme`
and other undocumented structures are app-managed; leave them exactly as
pulled.

Editing a whole book inside one JSON value is awkward. In the app, the
assistant edits the same books one chapter at a time at
`/handbooks/player/<chapter-id>.md` and
`/handbooks/builder/<chapter-id>.md` (frontmatter `title:` + markdown
body) — same data, per-chapter. From the workspace you have the JSON, so
rewrite only the chapter object you're changing and leave the rest of the
array byte-identical. Before writing one, read the writing-handbooks topic.

## `.craft/file-types/<slug>.json` — per-type image settings

Every type file carries the type's own image generation settings next to
its schema and layout:

- **`imageStyleInstructions`** — overrides the project style for images
  generated FOR files of this type (`craft image generate --for <path>`,
  and the app's per-file generation). Use it when one type needs its own
  look: portraits framed differently than locations, item icons on plain
  backgrounds, etc. `null` = inherit the project style.
- **`showCoverImage`** — whether files of this type display a cover
  image in the app at all. Set `false` for abstract record types
  (rules, tables, stat blocks) so they don't render as image-less cards.

Type settings push with `craft push --include-types` (same explicit gate
as schema edits) — or ride a platform import, which applies type changes
by default. A settings-only edit is safe: it never revalidates or
migrates the type's files.

## The layering, in one line

`craft image generate` composes: type style (with `--for`) → else
project style → else nothing; `--no-style` skips both. So: set the
project style once, add type overrides sparingly, and write every prompt
as SUBJECT ONLY.
