---
name: craft-file-types-and-schemas
description: "Read this BEFORE creating a new file type or editing any schema in .craft/file-types/ — covers craft type new, schema field types and constraints, server-injected base fields, and how craft push --include-types validates schema changes against every existing file."
---

# File Types & Schemas (workspace)

## Overview

File types define what kinds of files exist in this project. Each type is one
config file:

```
.craft/file-types/<slug>.json
```

This is part of the game's design, not just form configuration. Files hold the
world's content and changing state; a file type is their reusable blueprint;
its schema is the shared vocabulary and validation contract used by creators,
players, and the AI GM. If a rule expects the GM to remember or update a value,
model that value in a schema-backed file instead of leaving it implicit in
prose. The layout is separate presentation: the schema says what the data
means, the layout says how people see it, and each file holds the actual data.

```json
{
  "fileTypeId": "0197…",        // server-minted — never invent or edit
  "slug": "quest",
  "name": "Quest",
  "contentType": "json",        // "json" or "markdown"
  "designation": null,          // platform behavior hook — see "Designations" below; null for most types
  "category": null,
  "schemaHash": "…",            // server-minted concurrency token — never edit
  "schema": { … },              // JSON Schema for json types; null for markdown (a json type with null schema validates base fields only)
  "layout": null,               // rendering spec — see the designing-layouts instruction
  "imageStyleInstructions": null, // per-type image style (null = inherit project's) — see configuring-your-project
  "showCoverImage": true,       // whether files of this type show a cover image in the app
  "creationFlow": null          // guided-creation wizard (flow manifest) — see authoring-flows
}
```

A type file WITHOUT `fileTypeId`/`schemaHash` is a **locally authored new
type not yet pushed**. That absence is the signal — never add those keys by
hand, and never copy them from another type.

When auditing or broadly improving an existing type ("audit my file types",
"improve/clean up this schema") — or when a schema shows bad patterns like
duplicated derived fields, numbered slots, per-level/per-class branches,
resource counters, or presentation strings built by expressions — read
`.craft/instructions/refactoring-file-types.md` before editing. Treat that
work as a coordinated schema, content, referenced-definition, and layout
migration.

The **slug** is the display name lowercased with spaces replaced by dashes
("Quest" → `quest`, "Side Quest" → `side-quest`). The server verifies
slug === normalized name on push. The slug is also the extension on files of
the type: `SaveTheVillage.quest.json` (or `Anything.<slug>.md` for
markdown types).

## Creating a new file type

1. Scaffold it:

   ```
   craft type new "Quest"                          # json type
   craft type new "Lore" --markdown                # markdown type (no schema)
   craft type new "Monster" --designation character
   ```

   This writes `.craft/file-types/<slug>.json` with an empty starter schema.
   If it reports the slug already exists, run `craft pull` first (the type
   may exist remotely) or pick a different name.

2. Edit the `schema` in that file (json types). Define ONLY type-specific
   fields — see "Base fields" below.

3. Create files of the type anywhere in the workspace as
   `<Name>.<slug>.json` (or `.md`). Unlike creating a type in the app,
   pushing a new type does NOT auto-create a root folder — make a folder
   yourself (e.g. `Quests/`) if you want one; new directories push
   automatically.

4. Push the type and its files together in one atomic batch:

   ```
   craft push --include-types -m "add Quest type"
   ```

   A plain `craft push` never touches file types — type pushes are always
   explicit. `duplicate_slug` on push means the type now exists remotely:
   `craft pull` to adopt it, or rename yours.

## Designations

`designation` is NOT a free-form tag — it wires the type into platform
behavior, and the server rejects anything outside this exact list:

- `"character"` — files can be playable characters (assignable to game
  players, get an AI `voice` field, expressions/portrait features)
- `"location"` — files power map features
- `"gm_instructions"` — files become in-game GM rule documents
- `"game_start"` — the type behind Play's opening configuration. Standard
  projects ship one; an `--empty` project does NOT, and authoring one is
  the supported route there: `craft type new "Game Start" --designation
  game_start`, keep the schema empty (base fields only — the real game
  start config is designation metadata set with `craft meta
  set-game-start`, see making-it-playable). Don't author a second one in
  a project that already has the type.
- `null` — everything else. **Default to null.** A "quest", "faction", or
  "spell" type needs no designation; it changes nothing about how the type
  renders or validates.

## Base fields — never define them yourself

Every file of every type gets a baseline injected **server-side**:

- `name` (required, non-empty string) — the display name and file identity
- `description` (optional string)
- `image` (optional ImageObject)

Do NOT add these to `schema.properties` or `schema.required`. After a
successful `push --include-types` the server rewrites your local type file
with its stored, normalized copy — the base fields WILL appear in the schema
at that point. That is expected; leave them in place afterwards. Some
designations inject more (e.g. `"character"` types also gain a `voice`
field) — same rule: expected, leave it.

## Design principles

Get the shape right at creation time — these are the patterns audits later
exist to undo:

- **Classify every field before adding it**: stored state (a choice or
  mutable fact), a definition that belongs on a referenced type, a
  computed projection (expression field), or presentation. Presentation —
  labels, icons, checkbox glyphs, dot strings, formatted text — is the
  layout's job; never model it as a schema field.
- **Never store what can be computed.** A value derivable from other
  fields or referenced files is an expression field; a hand-entered copy
  goes stale the first time a source changes.
- **Collections, not numbered fields.** `items` with `maxItems: 3`,
  never `item1`/`item2`/`item3`; an object keyed by stable slot names
  when slots carry distinct meaning.
- **One map plus one computed array for per-item families.** Eighteen
  skills do not need a stored flag, a computed modifier, and a display
  field each — store one slug-keyed map and compute one array (the
  skill/resource idioms in
  `.craft/instructions/references-and-computed-fields.md`).
- **Reusable rules live on the type that owns them.** Progressions,
  resource definitions, and modifiers are data rows on the class, species,
  or item type; other types reference them and compute.
- **Identify things by stable keys and references, not display names** —
  names are editable content, not identifiers.

## Schema field types

- `string`, `number`, `boolean` — scalars
- `array` — describe entries with `items`
- `object` — describe nested fields with `properties`
- **image** — `{ "type": "image" }` (normalizes to the canonical
  `{ "type": "object", "format": "image" }`; both are accepted). Never use
  a plain `string` for image URLs. Stored values are ImageObjects
  (`{ url, focalPoint, overlayUrl, generation }`); never fabricate URLs.
- **enum** — `{ "type": "string", "enum": [ … ] }`, max 50 values. Use a
  reference to another file type for bigger vocabularies.
- **reference** — a field that points at another file:
  `{ "type": "string", "referencedFileTypeSlug": "armor" }`, or the
  `reference.targets` envelope for multi-type fields. Details in the
  references-and-computed-fields instruction. When designing a multi-target
  field, include EVERY type content will plausibly point at — a "subject"
  field that omits a type forces writers into prose mentions with no
  structured link (widening the target list later is a safe schema
  change; writers should flag the gap, not edit a shared schema
  mid-batch).
- **expression** — a computed value: a normally-typed field with an
  `expression` key, e.g.
  `{ "type": "number", "expression": "2 + floor(($self.level - 1) / 4)" }`.
  Top-level properties only; never add computed fields to `required`;
  their values are computed at render time and never stored in file
  content. Details in the references-and-computed-fields instruction.

### Constraints and defaults

Standard constraint keywords are enforced when content is pushed:
`minLength`/`maxLength` on strings, `minimum`/`maximum` on numbers,
`minItems`/`maxItems` on arrays. Scalar fields (string, number, boolean,
enum) also support `default`, applied only when a NEW file is created
without the field (never re-applied on updates); the default must satisfy the
field's own constraints.

```json
{
  "level":  { "type": "number", "minimum": 1, "maximum": 20, "default": 1 },
  "status": { "type": "string", "enum": ["open", "active", "complete"], "default": "open" },
  "tags":   { "type": "array", "items": { "type": "string" }, "maxItems": 10 }
}
```

## Updating an existing type's schema

Edit `schema` (and/or `layout`) in `.craft/file-types/<slug>.json`, then
`craft push --include-types`. What happens on the server:

- The push is guarded by the `schemaHash` you last synced at.
  `schema_hash_mismatch` means the type changed remotely — `craft pull`,
  re-apply your edit, retry.
- A schema change is validated against **every existing file of the type**,
  including files you cannot see locally. A rejection
  (`files_fail_new_schema`) lists exactly which files fail and why, with
  field-level errors. Fix the data first (edit those files and push them), or
  widen the schema — e.g. keep a new field optional instead of required.
- Layout-only changes skip that dataset validation entirely (layout is
  presentation) — safe to iterate on freely.

Adding optional fields is always safe. Removing fields or adding to
`required` is where existing files break — grep the workspace for files of
the type and update them in the same push when needed. In an atomic push
(the default), the whole batch is judged on POST-batch truth: the dataset
check counts same-batch updates and deletes, and new/updated file content
validates against the schema this same push establishes — so "change the
schema + add files using the new values" lands as one
`craft push --include-types`. (`--partial` gives up that guarantee:
each op stands alone there.) Content fields not declared in
`schema.properties` are allowed and stored as-is — schemas constrain what
they declare, nothing more.

## What you CANNOT change from the workspace

For an already-synced type, `schema`, `layout`, the image settings
(`imageStyleInstructions`, `showCoverImage`), and `creationFlow`
push. (`name`,
`contentType`, `designation`, and `category` do push for a brand-new
locally authored type, as part of its creation.) Everything else is
app-managed:

- **Renaming a type**, changing its `designation`/`category`/
  `contentType`, or **deleting a type** — do these in the Craft app; the
  next `craft pull` picks them up. Editing those keys in the local type
  file of a synced type does nothing on push (it is not even detected as
  drift).
- **States** (character expressions) and creation flows — these live
  server-side and never appear in the workspace type file. Do not add
  unknown keys to the file; the workspace refuses to load a type file with
  keys outside the documented shape.

## After any type push

The server echoes the stored type back and the CLI rewrites
`.craft/file-types/<slug>.json` from it (server-minted ids, recomputed
`schemaHash`, normalized schema with base fields merged). Expect the file to
change; do not "fix" it back.
