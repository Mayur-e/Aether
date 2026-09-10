---
name: craft-references-and-computed-fields
description: "Read this when a schema has (or you are adding) reference fields that point at other files, or computed fields (declared with an expression key). Covers the hydrated {slug, referenceId, fileTypeSlug} form in cloned files, writing new references by slug, referencing not-yet-pushed files, multi-target qualified slugs, the expression language, and why computed values never appear in file content."
---

# Reference & Computed Fields (workspace)

## Reference fields

A reference field points at another file. It is declared in the type schema
(`.craft/file-types/<slug>.json`) with `referencedFileTypeSlug` (single
target type) or a `reference.targets` envelope (multiple target types):

```json
{
  "armor": { "type": "string", "referencedFileTypeSlug": "armor" },
  "knownSpells": {
    "type": "array",
    "items": { "type": "string", "referencedFileTypeSlug": "spell" }
  },
  "loot": {
    "type": "array",
    "items": {
      "type": "string",
      "reference": {
        "targets": [{ "fileTypeSlug": "weapon" }, { "fileTypeSlug": "item" }]
      }
    }
  }
}
```

### What you see in cloned files (and must not "fix")

Cloned and pulled files contain references in the **stored, hydrated form**:

```json
{
  "armor": {
    "slug": "leather-armor",
    "referenceId": "0197a4c2-…",
    "fileTypeSlug": "armor"
  }
}
```

Every reference shape stores this same hydrated object once it resolves —
top-level refs, refs nested inside objects, each entry of a reference array,
and multi-target fields (where `fileTypeSlug` records which target type
matched). Leave hydrated objects exactly as they are unless you are changing
which file the field points at. `referenceId` is the rename-safe durable
pointer; the `slug` is display convenience. Pushing a file back verbatim
always validates — the server accepts both the hydrated form and the
plain-slug form. If the target was renamed since hydration, the stored
`slug` text is stale until the next save of the *referencing* file — any
push that touches it refreshes the slug from the live target (reported as
`normalized by server`). A `referenceId` whose target was deleted stays
as-is: dangling references are tolerated, never scrubbed. `craft check`
warns (`dangling-references`) about stored slugs that match no workspace
file's derived slug — the typo'd-slug case that would otherwise never
surface.

### Writing a new reference

Write the **target file's slug as a plain string**:

```json
{ "armor": "leather-armor", "knownSpells": ["mage-armor", "magic-missile"] }
```

A file's slug derives from its name (the filename stem): lowercase,
quotes/apostrophes dropped, every other run of non-alphanumeric characters
collapsed to a single dash, no leading/trailing dash ("Nature's Tongue" →
`natures-tongue`, "Bone & Blade" → `bone-blade`). Prefer copying an
already-hydrated `{ "slug": … }` from another file over deriving by hand.

The server resolves the slug on push and stores the hydrated object. After a
push, `craft push` reports `normalized by server: <path>` and rewrites
your working file to the stored form — expected, not a conflict.

**Never invent a `referenceId` value.** Either write a slug, or copy a real
referenceId from `.craft/index.json`.

An unused optional reference field is **omitted entirely** — never set it to
`""` (write `null` only to clear a previously set optional field).

For **multi-target** fields, qualify the slug as `"<type>/<slug>"` (e.g.
`"loot": ["weapon/dagger", "item/rope"]`). A bare slug that exists in more
than one target type stays an unresolved draft. The qualified string is only
the *write* form — once resolved it stores as the same hydrated object as
any other reference. (Files pushed before mid-2026 may still carry raw
`"type/slug"` strings in multi-target fields; leave them — they hydrate on
the next save of that file.)

A slug whose target file does not exist yet is tolerated: it is stored as an
incomplete `{ "slug": "…" }` and hydrates on a later save once the target
exists. Never block on this, but prefer creating the target first. (This
tolerance is JSON-reference-only — a markdown mention `<@slug.type>` whose
target does not exist rejects the whole push.)

### Referencing a file that does not exist remotely yet

Two new local files can reference each other before either is pushed:

1. Create both files locally.
2. Run `craft status` — it mints a pending `referenceId` for every new
   file into the ledger (`.craft/index.json`, entries with
   `"pending": "create"`). The server honors these ids on create, so they
   are durable from that moment.
3. For JSON reference fields, just write the target's slug — no id needed.
   Both files push in one batch and the slug resolves within the project.
4. Where you need the id itself — markdown mentions (`<@referenceId>`) or
   `.craft/project.json`'s `rootMapFileReferenceId` — look up the pending
   referenceId in `.craft/index.json` by the file's path and use it.

References between files created in the same push batch hydrate like any
other: the server treats the batch's own creates as resolvable targets, so
both files land with full `{ slug, referenceId, fileTypeSlug }` objects.
(Only `craft push --partial` keeps same-batch refs in the compact
`{ "slug": "…" }` form — there a failed create must not leave a hydrated
pointer behind; they upgrade on a later save of the referencing file.)

### References are one-way — reverse links are not automatic

A reference from A to B is complete by itself: it hydrates on push and
nothing else is required. If the schema also has a roster on the other
side (a faction's `members` listing characters that declare that faction,
a location's notable residents), that is an ordinary reference field on
the other file — nothing back-fills it when you write the forward edge.

Whether a reverse roster is worth maintaining is a design choice; when a
schema has one, keep it consistent before calling the work done. Update
rosters as a **finishing pass** over each shared file, not per-file while
batch-writing — parallel batches editing one shared faction file collide.
One editor, once, at the end.

### Reference filters (multi-target)

Each `reference.targets` entry may carry a `filter` restricting which
files qualify. A filter is one comparison `{ "op", "left", "right" }` or an
`and`/`or` group of up to 3 in `args`. Operands:
`{ "scope": "target", "path": "<field>" }` (the candidate file),
`{ "scope": "source", "path": "<field>" }` (the file being edited), or a
bare literal. Ops: `eq`, `neq`, `in`, `nin`, `gt`, `gte`, `lt`,
`lte` — ranges only on numbers; `in`/`nin` on enum fields (max 10
listed values); filterable fields must be text, number, enum, boolean, or
reference; a target↔source comparison must be between fields of the same
type (references must share a target file type). A field can target at most
5 types.

```json
{
  "fileTypeSlug": "weapon",
  "filter": {
    "op": "and",
    "args": [
      { "op": "in",  "left": { "scope": "target", "path": "rarity" }, "right": ["common", "rare"] },
      { "op": "lte", "left": { "scope": "target", "path": "value" },  "right": { "scope": "source", "path": "budget" } }
    ]
  }
}
```

Reference fields compare by identity: `{ "op": "eq", "left": { "scope":
"target", "path": "class" }, "right": { "scope": "source", "path": "class" } }`
keeps a character's spell picker to spells whose `class` reference matches
the character's. When the target field is a LIST of references, use `in` with the list on the right —
`{ "op": "in", "left": { "scope": "source", "path": "class" }, "right":
{ "scope": "target", "path": "classes" } }` matches files whose `classes`
list includes the source's class (`nin` negates). Matching uses the stored
`referenceId` first, falling back to slug for draft references.

A pushed reference that fails its filter still saves (soft warning in the
app); a misconfigured filter is rejected when the type is pushed.

## Computed fields (the `expression` key)

A computed field is a **normally typed field with an `expression` key**.
The declared `type` is the output type — number, string, boolean, array, or
object:

```json
{
  "dexMod": {
    "type": "number",
    "expression": "floor((($self.dexterity ?? 10) - 10) / 2)"
  }
}
```

### Computed values are NEVER in file content

Expression values are computed at render time. The sync protocol strips them
from every write, and stored content never contains them — so cloned and
pulled files simply do not have computed fields in their JSON. That is
correct:

- Do not add a computed field's value to a data file. If you do, the push
  strips it silently.
- Do not add computed fields to `schema.required` — they are computed,
  not stored, and the type push rejects it.
- To change a computed value: change the source fields it reads, edit the
  referenced files, or change the `expression` in the type schema and
  `craft push --include-types`.

`craft check` validates both rules offline: `file-types` runs the same
schema and expression validation the server runs at push (fails on issues
a push would reject), and `computed-field-values` warns about values
stored under computed fields (which a push would strip silently). Run it
after editing type schemas, before pushing.

### Writing expressions

The language is a small JavaScript expression subset: literals, template
literals, arithmetic (`+ - * / % **`), comparisons, `===`/`!==`,
`&& || ??`, ternaries, member access/indexing, array/object literals, and
arrow lambdas **only as literal arguments to builtins** (`e => e.quantity`,
expression body only). No assignment, statements, globals (`Math`,
`Object`, ...), regex, or `new`.

- `$self` is the file's own content: `$self.level`,
  `$self.stats.strength`. Expressions may read other computed fields on
  the same type (dependency-ordered; cycles are rejected on push). The first
  hop off `$self` must be a static field name — `$self[key]` is rejected.
- **Top-level properties only.** An `expression` nested inside an object
  property or array `items` is rejected. For per-item computed values,
  make the whole array a computed field (see idioms below).
- Null-safety: a missing field, member access on `null`, or arithmetic on
  a non-number produces `null` instead of an error (division by zero too).
  Default with `??`: `($self.hp ?? 0) + bonus`. Equality is strict and
  deep over arrays/objects. Arithmetic never coerces numeric strings — use
  `number(x)` explicitly.

### load(value) — reading referenced files

Pass a reference field's **value** straight to `load()` — the hydrated
`{ slug, referenceId, fileTypeSlug }` object you see in cloned files — to
get the referenced file's content (with that file's own computed fields
computed, plus `$id`, `$name`, `$title`, `$image`, `$fileTypeSlug`):

- Single reference: `load($self.armor)?.acBonus`
- Refs inside array rows:
  `sum($self.inventory, e => (load(e.item)?.weight ?? 0) * (e.quantity ?? 1))`
- Array of references: `$self.knownSpells.map(s => load(s)?.school)`

The reference value itself is only an identity handle — it never carries the
target file's fields. `$self.armor.acBonus` is always `null`; the read
must go through `load($self.armor)?.acBonus`.

Cross-file reads are **one level deep**: the referenced file's own
expressions cannot use `load()` themselves — longer chains are
rejected on push, and reads off `load(...)` are checked to exist on
the target type. Unresolved references yield `null` (including a
not-yet-hydrated plain slug string before its first push round-trip), so
guard with `?.` and `??`.

### Builtins

- Globals: `floor ceil round abs sqrt pow min max clamp sum avg count
  number string boolean keys values entries has load`
- Array: `.length` (property), `map filter find findIndex some every
  includes indexOf at slice concat flat flatMap join reduce toSorted
  toReversed unique` (`at(-1)` = last element; lambdas get `(e, i)`)
- String: `.length`, `toUpperCase toLowerCase trim includes startsWith
  endsWith slice split replace replaceAll padStart padEnd`
- Template literal interpolation (backtick strings): `Level ${$self.level}`

### Idioms

Progression tables are real data, not strings — store the table as an array
(usually on the referenced class/subclass file) and pick the row:

```json
{ "type": "number", "expression": "(load($self.class)?.slotsTable ?? []).filter(r => r.level <= $self.level).at(-1)?.slots ?? 0" }
```

When per-item definitions (name, maximum) live on a referenced file but
current values live on this file, compute the whole array by zipping them —
change the class reference and the resources follow:

```json
{
  "resourceState": {
    "type": "object",
    "additionalProperties": { "type": "object", "properties": { "current": { "type": "number" } } }
  },
  "resources": {
    "type": "array",
    "items": { "type": "object", "properties": { "key": { "type": "string" }, "name": { "type": "string" }, "current": { "type": "number" }, "maximum": { "type": "number" } } },
    "expression": "(load($self.class)?.resources ?? []).map(r => ({ key: r.key, name: r.name, current: $self.resourceState?.[r.key]?.current ?? 0, maximum: r.maximum }))"
  }
}
```

Stored content holds only `resourceState` (e.g.
`{ "rage": { "current": 2 } }`); `resources` is derived on every read. Give
each row a stable slug `key` and key `resourceState` by it. In the layout,
repeat a tracker over the computed array and add
`"editPath": { "value": "/resourceState/{key}/current" }` — the control
displays the computed row but writes to the stored map, keeping the tracker
playable. Full pattern in the designing-layouts instruction ("Editable
trackers over computed arrays").

**The same zip collapses any per-item field family.** Skills, saves, or
senses never need a stored flag, a computed modifier, and a display field
each: store one slug-keyed map (`skillProficiencies`) and compute one
`skills` array from definition rows — on the rules/system file via
`load()`, or an inline literal array for a fixed vocabulary — reading
shared computed fields like `$self.proficiencyBonus` so each rule exists
exactly once:

```json
{
  "skillProficiencies": {
    "type": "object",
    "additionalProperties": { "type": "boolean" }
  },
  "skills": {
    "type": "array",
    "expression": "(load($self.ruleSet)?.skills ?? []).map(row => ({ key: row.key, name: row.name, proficient: $self.skillProficiencies?.[row.key] == true, modifier: floor((($self.abilityScores?.[row.ability] ?? 10) - 10) / 2) + ($self.skillProficiencies?.[row.key] == true ? ($self.proficiencyBonus ?? 2) : 0) }))"
  }
}
```

The layout renders one row repeated over the computed array — a `Toggle`
checkbox with `"editPath": { "value": "/skillProficiencies/{key}" }`
beside the computed modifier.

### Worked example — armor-driven AC

In `.craft/file-types/character.json`, add to `schema.properties`:

```json
{
  "armor": { "type": "string", "referencedFileTypeSlug": "armor" },
  "armorClass": {
    "type": "number",
    "expression": "10 + floor((($self.dexterity ?? 10) - 10) / 2) + (load($self.armor)?.acBonus ?? 0)"
  }
}
```

Push with `craft push --include-types`. Then in a character file:

```json
{ "armor": "leather-armor" }
```

The rendered view computes `armorClass` from the referenced armor's
`acBonus` — nothing else to write.
