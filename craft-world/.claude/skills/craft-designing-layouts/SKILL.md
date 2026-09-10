---
name: craft-designing-layouts
description: "Read this BEFORE creating, editing, or auditing the layout field of any .craft/file-types/<slug>.json — \"audit my layout\", \"improve this sheet\" — covers the layout spec format, every available component with its exact props and enum values, $state data binding, visibility conditions, repeat + editPath editable trackers, the layout audit workflow, and how to push layout changes safely."
---

# Designing File Type Layouts (workspace)

## Overview

Layouts are JSON specs stored on file types that control how files render on
their detail view in the Craft app. A type with a `layout` renders through
the json-render component system; `"layout": null` falls back to a plain
field-by-field display.

The schema is the durable meaning and writable state of a file; the layout is
the player-facing lens on that state. Layouts are one of the surfaces that give
Craft its visual-novel-like presentation, so design them around what a player
needs to recognize, decide, and change—not around dumping every stored field
onto the screen. Keep rules and state in schemas/files; never encode game logic
only in visual arrangement.

In the workspace, the layout lives in the `layout` field of
`.craft/file-types/<slug>.json`. The edit loop:

1. Read the type file — the `schema` tells you every bindable field.
2. Edit `layout` in the same file.
3. `craft push --include-types` — layout-only changes skip the
   schema-vs-existing-files validation, so iterating on a layout is always
   safe and fast.
4. View the result on a file of that type in the Craft app; `craft pull`
   brings back any layout edits made in the app.

**There is no safety net on this path.** Unlike the in-app assistant, pushes
from the workspace do NOT validate layouts against the component catalog: a
misspelled component name, a wrong prop, or an invalid enum value is stored
as-is and silently renders wrong or not at all. Copy component names, props,
and enum values EXACTLY from the tables below.

The layout field is authoritative: whatever is in the file is what the type
gets — nothing is auto-generated on push. (Only in-app schema edits
auto-generate default layouts.) So when a type deserves more than the plain
default display, write the layout by hand — that is the point of this
document. Before designing, also skim the other `.craft/file-types/*.json`
in the workspace: matching the project's existing layout style beats
inventing a new one.

## Layout spec format

A flat map of elements plus a root pointer:

```json
{
  "root": "page",
  "elements": {
    "page": {
      "type": "Stack",
      "props": { "direction": "vertical", "gap": "md" },
      "children": ["hero", "body"]
    },
    "hero": {
      "type": "ImageHeader",
      "props": { "src": { "$state": "/portrait" }, "title": { "$state": "/name" } },
      "children": []
    },
    "body": {
      "type": "Card",
      "props": {},
      "children": ["stats"]
    },
    "stats": {
      "type": "StatGrid",
      "props": {
        "items": [["Strength", { "$state": "/strength" }], ["Dexterity", { "$state": "/dexterity" }]],
        "cols": 2
      },
      "children": []
    }
  }
}
```

**Structural rules (violations break rendering):**

- `root` — string key pointing at the root element.
- `elements` — flat map of element-key → element. No nesting; hierarchy is
  expressed only through `children`.
- Each element: `type` (component name), `props` (object), `children`
  (array of element keys). Optional `visible` (condition, below).
- Every element MUST have a `"props": {}` key, even when empty.
- Every key in `children` MUST exist in `elements` — a missing child is
  invisible UI with no error.
- Enum props must use the EXACT listed values. `aspectRatio` is
  `"ultrawide"`/`"ultratall"`, never `"ultra-wide"`/`"ultra-tall"`.

## Data binding

File content fields are the state. Bind with `{ "$state": "/fieldName" }`
anywhere a prop takes a value:

```json
{ "text": { "$state": "/name" } }
{ "src": { "$state": "/portrait" } }
{ "items": { "$state": "/inventory" } }
```

Nested fields: `{ "$state": "/stats/strength" }` resolves
`content.stats.strength`.

Available paths come from the type's schema fields, plus these
platform-provided aliases:

- `/title` — mirrors `/name` (both work)
- `/description` — the file's description
- `/image` — the file's primary ImageObject (Image/ImageHeader components
  resolve `.url`, focal point, and generation state automatically)
- `/createdAt`, `/updatedAt` — timestamps
- `/_<fieldName>Entries` — for every object field, `Object.entries(field)`
  as `[key, value]` pairs; handy for `StatGrid`/`Table` when the object's
  keys are dynamic
- Expression (computed) fields are evaluated into state — bind them like
  any other field.

## Visibility conditions

Show/hide elements with `visible` on the element:

```json
{
  "type": "Image",
  "props": { "src": { "$state": "/portrait" }, "alt": "Portrait" },
  "visible": { "$state": "/portrait" },
  "children": []
}
```

Condition grammar:

- Truthy: `{ "$state": "/field" }`
- Negation: `{ "$state": "/field", "not": true }` (any condition may add
  `"not": true`)
- Equality: `{ "$state": "/field", "eq": "value" }` /
  `{ "$state": "/field", "neq": "value" }`
- Numeric: `{ "$state": "/field", "gt": 5 }` — also `gte`, `lt`, `lte`
- One comparison operator per condition — never combine `eq` with `gt` etc.
- AND: `{ "$and": [cond, cond] }` (or a bare array `[cond, cond]`)
- OR: `{ "$or": [cond, cond] }`

One nuance for the bare truthy form: when the truthy path is a field the
element (or its subtree) itself displays — like the Image example above —
the editor keeps the element rendered so the empty field stays editable
in place, and the component shows its own empty state instead. A truthy
gate on a field the subtree does NOT display (e.g. a boolean
`{ "$state": "/offersGear" }` hiding a gear section that displays
`/gear`) hides the element everywhere, as do all conditions with
comparison operators. Use that cross-field form for optional sections a
toggle controls.

## Repeating over arrays

Any element can render its children once per item of an array field with
`repeat`:

```json
{
  "trackers-list": {
    "type": "Stack",
    "props": { "direction": "vertical", "gap": "sm" },
    "repeat": { "statePath": "/trackers", "key": "key" },
    "children": ["tracker-row"]
  },
  "tracker-row": {
    "type": "Dots",
    "props": {
      "label": { "$item": "label" },
      "value": { "$item": "current" },
      "max": { "$item": "max" }
    },
    "children": []
  }
}
```

Inside a repeat scope, bind props with `{ "$item": "field" }` (a field on
the current item) or `{ "$item": "" }` (the whole item); `visible`
supports `$item`/`$index` conditions per row. `key` names an item field
with a stable unique value. `repeat.statePath` is absolute — nested repeats
are not supported. Inline edits on `$item`-bound props save back to that
item's row in the array. Prefer ONE array field plus `repeat` over unrolled
per-item fields.

### Editable trackers over computed arrays (editPath)

This is THE recommended pattern for class resources
(rage, ki, spell slots): definitions and maximums live on the referenced
class file, current values in a stored map field on this file, and a
computed `resources` array zips them (the computed-resource-array idiom in
the references-and-computed-fields instruction). Computed values are read-only, so
a control repeating over the computed array cannot save back to its rows —
`props.editPath` redirects where the editable prop writes:

```json
{
  "resource-list": {
    "type": "Stack",
    "props": { "direction": "vertical", "gap": "sm" },
    "repeat": { "statePath": "/resources", "key": "key" },
    "children": ["resource-row"]
  },
  "resource-row": {
    "type": "Dots",
    "props": {
      "label": { "$item": "name" },
      "value": { "$item": "current" },
      "max": { "$item": "maximum" },
      "editPath": { "value": "/resourceState/{key}/current" }
    },
    "children": []
  }
}
```

`editPath` maps an editable prop name to an absolute state-path template.
The control still displays the bound (possibly computed) value; edits save
to the resolved template path instead. `{key}` placeholders read that
field from the current repeat item, so each computed row must carry a
stable slug field matching the stored map's keys — the backing store is a
stored map field keyed by stable slugs (`resourceState` above, an object
with `additionalProperties`), never by row index or display name.

Rules:

- Templates are absolute state paths; `{placeholders}` must be identifier
  names. The app rejects malformed templates when a layout is saved there,
  but `craft push` does NOT validate them — copy the shape exactly.
- Templates with placeholders only resolve inside a `repeat`;
  placeholder-less templates work anywhere. Placeholder values must be
  non-empty strings or finite numbers on the repeat item. When resolution
  fails, the control renders read-only instead of writing somewhere wrong.
- The resolved target must be a genuinely STORED field — a target on or
  under a computed (expression) field is refused and the control
  stays read-only.
- The target need not pre-exist in stored content; missing parents are
  created on the first save (`{ "rage": { "current": 2 } }` appears when
  the player first clicks).
- Supported on the editable props of `Dots`, `Stepper`, `Progress`,
  `TrackBoxes`, `Clock`, `Toggle`, and `Text`.

Footguns:

- **Always pair editPath with a binding that re-derives from the written
  path.** editPath makes the prop editable even when the display value is a
  literal or an unrelated binding — the save works but the display never
  reflects it. The computed array must read the same stored path back
  (`current: $self.resourceState?.[r.key]?.current ?? 0`).
- **Digit-only resolved segments act as array indexes** —
  `/resourceState/3/current` writes into an array, not an object key. Use
  non-numeric keys (`rage`, `slots-1`) for map-style storage.

Outside a repeat, a single tracker for one known resource needs no
editPath: bind `value` directly to the stored path
(`{ "$state": "/resourceState/rage/current" }`, editable as-is) and bind
`max` to a computed per-key object field (e.g. `/resourceMaxByName/rage`)
— that computed-object bridge remains valid for max display and visibility
gating outside repeat.

## Available components

Props marked `?` are optional/nullable. Color props (`color`, `bgColor`,
`borderColor`, `trackColor`, `labelColor`, `valueColor`) accept any CSS
color value. `rounded?` is one of `"none"`/`"sm"`/`"md"`/`"lg"`/`"xl"`/`"2xl"`/`"full"`.
`aspectRatio` values everywhere: `"video"` (16:9), `"square"` (1:1),
`"portrait"` (3:4), `"tall"` (9:16), `"ultrawide"` (32:9), `"ultratall"` (9:32).

Stack, Grid, and Card also accept `bgImage?` (a background image URL, or a
binding to an image field) plus `bgImageFit?` — `"cover"` (default, crop to
fill), `"contain"` (fit inside), or `"tile"`/`"tile-x"`/`"tile-y"` (repeat at
natural size for seamless textures). Use `bgImage` for background artwork; a
Tailwind url() class in `className` cannot work, because the stylesheet is
compiled at build time.

### Layout

| Component | Props | Notes |
|-----------|-------|-------|
| `Stack` | `direction?` ("horizontal"/"vertical"), `gap?` ("sm"/"md"/"lg"/"none"), `align?` ("start"/"center"/"end"/"stretch"), `justify?` ("start"/"center"/"end"/"between"/"around"), `wrap?` (bool), `bgColor?`, `bgImage?`, `bgImageFit?`, `rounded?`, `className?` | Flex container. `className` takes extra Tailwind utilities (e.g. `col-span-2`, `mt-4`). |
| `Grid` | `columns?` (1–12), `gap?` ("sm"/"md"/"lg"), `bgColor?`, `bgImage?`, `bgImageFit?`, `rounded?`, `className?` | CSS grid; children may use `className: "col-span-*"`. |
| `Card` | `title?`, `description?`, `maxWidth?` ("sm"/"md"/"lg"/"full"), `centered?` (bool), `bgColor?`, `borderColor?`, `bgImage?`, `bgImageFit?`, `rounded?`, `className?` | Container card. |
| `Separator` | `orientation?` ("horizontal"/"vertical") | Divider line. |
| `Tabs` | `tabs` [{label, value}], `defaultValue?`, `value?`, `bgColor?` | Children map to panels by position (first child = first tab). |
| `Accordion` | `items` [{title, content}], `type?` ("single"/"multiple") | Collapsible text sections; content is a string. |
| `Collapsible` | `title`, `defaultOpen?` (bool) | Expandable area; children render inside. |
| `Table` | `columns` (string[]), `rows` (string[][]), `caption?` | `rows` is a 2D array of cell strings, e.g. `[["Alice","admin"],["Bob","user"]]`. |

### Content

| Component | Props | Notes |
|-----------|-------|-------|
| `Heading` | `text`, `level?` ("h1"/"h2"/"h3"/"h4"), `color?` | Defaults to h2. |
| `Text` | `text`, `variant?` ("caption"/"body"/"muted"/"lead"/"code"), `color?` | Paragraph text. |
| `Image` | `src`, `alt?`, `size?` ("sm"/"md"/"lg"/"full"), `align?` ("left"/"center"/"right"), `aspectRatio?` | `src` is required (bind an image field); omit `aspectRatio` to keep the natural ratio. |
| `Badge` | `text`, `variant?` ("default"/"secondary"/"destructive"/"outline"), `color?` | `color` overrides variant with auto-contrasted text. |
| `Progress` | `value`, `max?`, `label?`, `color?`, `trackColor?`, `showValue?` (bool), `stepper?` (bool) | `showValue` prints "value / max"; `stepper` adds hover −/+ buttons that bump the value by 1 — use it instead of pairing a bar with a separate Stepper for the same field. |
| `Avatar` | `src?`, `name`, `size?` ("sm"/"md"/"lg") | Falls back to initials from `name`. |
| `Alert` | `title`, `message?`, `type?` ("info"/"success"/"warning"/"error") | Callout box. |

### Craft custom

| Component | Props | Notes |
|-----------|-------|-------|
| `Markdown` | `content`, `color?` | Prose-styled markdown; use for long-text fields. |
| `StatGrid` | `items` [[label, value], …], `cols?`, `color?`, `bgColor?` | Grid of labeled stats; each item is a [string, string\|number] tuple. |
| `DerivedStatGrid` | `items` [[label, base, derived], …], `baseLabel?`, `derivedLabel?`, `cols?`, `color?`, `bgColor?` | Pairs an editable base value with a derived one (e.g. ability score + modifier). |
| `BadgeList` | `items` (string[]), `options?` (string[]), `variant?` ("default"/"secondary"/"outline"/"destructive"), `color?`, `fileTypeSlug?`, `fileTypeSlugs?` (string[]), `showAddNew?` (bool), `showAddNewOnHover?` (bool) | Wrapped badges for string arrays. `options` adds a preset picker for fixed vocabularies; `fileTypeSlug` turns it into a compact editable reference list (`fileTypeSlugs` when the field targets multiple types). `showAddNew` and `showAddNewOnHover` default true. |
| `Checklist` | `items`, `labelField?`, `checkedField?`, `descriptionField?`, `metaFields?` [{label, field}], `showProgress?` (bool), `emptyText?` | Read-only checklist for task/objective arrays; items can be strings or objects. |
| `Dots` | `value`, `max?`, `label?`, `color?`, `emptyColor?`, `size?` ("sm"/"md"/"lg"), `showValue?` | Filled/empty dot tracker for small pools (spell slots, ki, uses per rest). Bind `value` to a number field — clicking a dot sets the value. `max` defaults to 5, caps at 25. |
| `Stepper` | `value`, `label?`, `min?` (default 0), `max?`, `step?`, `showMax?`, `color?`, `size?` | −/+ counter for one-tap turn-time edits (HP, ammo, points). Each tap saves; value is also click-to-type. |
| `Toggle` | `value`, `label?`, `variant?` ("switch"/"chip"/"checkbox"), `onLabel?`, `offLabel?`, `color?` | One-tap boolean (inspiration, concentration, flags). Clicking flips and saves. |
| `TrackBoxes` | `boxes?` (default 10), `value?` OR `states?`, `stateOptions?` [{label, symbol}], `groupEvery?`, `label?`, `color?`, `size?`, `showValue?` | Square box track. Bind `value` (number) for filled-count, or `states` (string array) for multi-state cycling (WoD bashing/lethal/aggravated, Blades stress). |
| `ConditionToggles` | `value` (string array), `options?` [strings or {label, value?, color?}], `label?`, `color?` | Always-visible chip roster; clicking toggles membership in the bound array. For conditions/status effects. |
| `Clock` | `value`, `segments?` (2–12, default 4), `label?`, `color?`, `emptyColor?`, `size?`, `showValue?` | Segmented circular progress clock (Blades style) for countdowns and project clocks. Click a segment to fill up to it. |
| `DataTable` | `items` (object array), `columns` [{label, field, type?}], `label?`, `emptyText?` | Bindable table with inline-editable cells for structured arrays (weapons, inventory rows). For reference arrays prefer `FileReferenceGrid`. |
| `FieldList` | `items` [[label, value], …], `labelColor?`, `valueColor?` | Vertical label/value list (string values). |
| `ImageHeader` | `src` (nullable), `title?`, `subtitle?`, `aspectRatio?` | Hero image with text overlay; renders a gradient fallback when no image — safe without a `visible` guard. `aspectRatio` defaults to "video". |
| `FormattedDate` | `value`, `format?` ("date"/"relative"/"distance"), `color?` | "April 29, 2024" / "yesterday" / "2 days ago". |
| `FormattedNumber` | `value`, `style?` ("decimal"/"currency"/"percent"), `currency?`, `color?` | `currency` is an ISO code like "USD". |
| `Select` | `value`, `label?`, `placeholder?`, `options?` [{label, value}], `fileTypeSlug?`, `fileTypeSlugs?` (string[]) | Dropdown. Static `options`, or `fileTypeSlug` to populate from files of that type (`fileTypeSlugs` when the field targets multiple types). |
| `VoiceSelect` | `value`, `label?`, `placeholder?`, `showLabel?` (bool) | Voice picker for character dialogue; bind to a string field with format "voice". |
| `FileReferenceGrid` | `items`, `fileTypeSlug`, `fileTypeSlugs?` (string[]), `referenceField?`, `columns?`, `size?` ("sm"/"md"/"lg"), `imageAspectRatio?`, `label?`, `metadataFields?`, `emptyState?` (string), `showAddNew?` (bool), `showAddNewOnHover?` (bool), `addNewVariant?` ("inline"/"aside") | Cards for arrays of referenced files, with built-in add/remove picker. `fileTypeSlugs` lists every target type for multi-type reference fields. `emptyState` defaults to "None.". `showAddNew` and `showAddNewOnHover` default true; `addNewVariant` defaults "inline", while "aside" places one compact trigger beside the label. See below. |
| `TurnTracker` | `combatants`, `stats` [{label, field, maxField?, display?}], `round?`, `currentTurnIndex?`, `fileTypeSlug?`, `pageFieldMapping?`, `conditionsField?`, `notesField?` | Encounter tracker; `display` is "bar"/"fraction"/"badges"/"text". |

Do not use `PageReferenceGrid` — it is a deprecated alias for
`FileReferenceGrid`.

## Reference components

Reference behavior must be declared in the SCHEMA first — the field that
stores the reference carries `referencedFileTypeSlug`:

```json
{
  "armor": { "type": "string", "referencedFileTypeSlug": "armor" },
  "inventory": {
    "type": "array",
    "items": {
      "type": "object",
      "properties": {
        "item": { "type": "string", "referencedFileTypeSlug": "item" },
        "quantity": { "type": "number" }
      }
    }
  }
}
```

The schema annotation is the durable source of truth for computed fields
(`load($self.armor)` only works on annotated reference fields), backlinks,
and editing surfaces. A layout hint alone can mark a field as a reference for
display, but always keep schema and layout in agreement.

Single reference field → `Select` with `fileTypeSlug`:

```json
{
  "type": "Select",
  "props": { "value": { "$state": "/armor" }, "label": "Armor", "fileTypeSlug": "armor" },
  "children": []
}
```

Array of references → `FileReferenceGrid`:

```json
{
  "type": "FileReferenceGrid",
  "props": {
    "items": { "$state": "/inventory" },
    "fileTypeSlug": "item",
    "referenceField": "item",
    "columns": 4,
    "size": "md",
    "label": "Inventory"
  },
  "children": []
}
```

For object arrays, `referenceField` names the property holding the
reference; sibling properties (quantity, equipped, …) render as editable
metadata badges. A single scalar "equipment slot" can reuse the grid by
wrapping the binding: `"items": [{ "$state": "/mainHand" }]` with
`"referenceField": null`.

Multi-type reference field (schema `reference.targets` envelope — see the
references instruction) → the same components with `fileTypeSlugs`:

```json
{
  "type": "FileReferenceGrid",
  "props": {
    "items": { "$state": "/loot" },
    "fileTypeSlugs": ["weapon", "item"],
    "referenceField": null,
    "columns": 4,
    "label": "Loot"
  },
  "children": []
}
```

The add picker gains per-type filter chips and type badges on results, and
picked values store as qualified `"type/slug"` strings. `BadgeList` and
`Select` accept `fileTypeSlugs` the same way. When the schema already
declares the targets, components infer them and the slug props may be
omitted; generated layouts still write them explicitly.

`FileReferenceGrid` and `BadgeList`-with-`fileTypeSlug` come with
built-in add/remove pickers — never build custom editing controls into a
layout. Their add controls, and the plain `List` add control, are visible on
hover on desktop and always visible on touch devices. Set
`showAddNewOnHover: false` to keep Add visible on desktop, or
`showAddNew: false` when the layout must never offer creation; existing-item
editing remains available.

## Full example — Character

Schema (type-specific fields; base name/description/image are injected):

```json
{
  "type": "object",
  "properties": {
    "class":     { "type": "string" },
    "level":     { "type": "number", "minimum": 1, "default": 1 },
    "portrait":  { "type": "image" },
    "strength":  { "type": "number" },
    "dexterity": { "type": "number" },
    "wisdom":    { "type": "number" },
    "backstory": { "type": "string" },
    "abilities": { "type": "array", "items": { "type": "string" } }
  },
  "required": ["class"]
}
```

Layout (the complete `layout` value in `.craft/file-types/character.json`):

```json
{
  "root": "page",
  "elements": {
    "page": {
      "type": "Stack",
      "props": { "direction": "vertical", "gap": "none" },
      "children": ["hero", "body"]
    },
    "hero": {
      "type": "ImageHeader",
      "props": {
        "src": { "$state": "/portrait" },
        "title": { "$state": "/name" },
        "subtitle": { "$state": "/class" },
        "aspectRatio": "video"
      },
      "children": []
    },
    "body": {
      "type": "Card",
      "props": {},
      "children": ["body-stack"]
    },
    "body-stack": {
      "type": "Stack",
      "props": { "direction": "vertical", "gap": "md" },
      "children": ["stats", "sep1", "abilities-section", "backstory-section"]
    },
    "stats": {
      "type": "StatGrid",
      "props": {
        "items": [
          ["Level", { "$state": "/level" }],
          ["Strength", { "$state": "/strength" }],
          ["Dexterity", { "$state": "/dexterity" }],
          ["Wisdom", { "$state": "/wisdom" }]
        ],
        "cols": 4
      },
      "children": []
    },
    "sep1": {
      "type": "Separator",
      "props": {},
      "children": []
    },
    "abilities-section": {
      "type": "Stack",
      "props": { "direction": "vertical", "gap": "sm" },
      "visible": { "$state": "/abilities" },
      "children": ["abilities-heading", "abilities-list"]
    },
    "abilities-heading": {
      "type": "Heading",
      "props": { "text": "Abilities", "level": "h3" },
      "children": []
    },
    "abilities-list": {
      "type": "BadgeList",
      "props": { "items": { "$state": "/abilities" }, "variant": "secondary" },
      "children": []
    },
    "backstory-section": {
      "type": "Stack",
      "props": { "direction": "vertical", "gap": "sm" },
      "visible": { "$state": "/backstory" },
      "children": ["backstory-heading", "backstory-text"]
    },
    "backstory-heading": {
      "type": "Heading",
      "props": { "text": "Backstory", "level": "h3" },
      "children": []
    },
    "backstory-text": {
      "type": "Markdown",
      "props": { "content": { "$state": "/backstory" } },
      "children": []
    }
  }
}
```

Then `craft push --include-types` and open a character in the app.

## Auditing an existing layout

Use this workflow when the user asks to review or improve how a type's
files look, and as the layout pass of a full file-type audit (for the
combined schema + layout audit, read
`.craft/instructions/refactoring-file-types.md`). The goal is a sheet
where everything a player looks up is visible at a glance and everything
they change during play is editable in one tap, right where it is shown.

1. **Read schema and layout together.** Read
   `.craft/file-types/<slug>.json` once — every judgment pairs a field's
   role (stored state, computed projection, reference, presentation) with
   how the layout presents it.

2. **Walk every binding, in both directions.**
   - Layout → schema: every `$state` path, `$item` field, repeat
     `statePath`, `editPath` target, `SectionSwitch` `statePath`, and
     visibility condition must resolve to a real schema field or metadata
     alias. A stale path renders blank with no error — treat dead bindings
     as breakages, not polish.
   - Schema → layout: list fields that never appear in the layout. Decide
     which are deliberately hidden (state maps behind computed displays,
     internal keys) and which are gaps a player would miss.

3. **Check play-time interactivity.** Anything that changes during play
   should save in one tap where it is displayed: `Dots` or `TrackBoxes`
   for pools, `Stepper` or `Progress` with `stepper` for counters,
   `Toggle` for booleans, `ConditionToggles` for status arrays,
   `Clock` for countdowns. Computed values render read-only; a tracker
   repeating over a computed array needs `editPath` into the stored state
   map — and flag any editPath whose display binding does not re-derive
   from the written path.

4. **Check structure and duplication.** Numbered copies of the same
   element block become one `repeat`. Per-class or per-subclass blocks
   behind visibility gates become `sections` with a `SectionSwitch`.
   The same value shown twice is shown once. A long vertical dump becomes
   two to five task-oriented tabs (identity, play stats, resources,
   inventory, notes), with always-needed identity kept outside the tabs.

5. **Match component to field role.** Display-string fields bound to
   `Text` become the underlying typed field on the matching interactive
   component; reference fields get `Select` or `FileReferenceGrid`;
   label/value walls become `StatGrid` or `DerivedStatGrid`; long prose
   gets `Markdown`.

6. **Report findings like the file-type audit: plain language, outcomes
   first.** "Spell slots become dots you tap during play", "the sheet
   condenses into three tabs" — no element keys, no JSON, one recommended
   plan, exactly one yes-or-no question. Layout-only changes touch no
   stored content, so say so: none of the user's files need migrating.

7. **Verify before and after pushing.** Local files carry stored content
   only — computed values exist server-side — so check binding paths
   against the schema by hand, then run `craft check` to validate the
   spec and `craft push --include-types` after the user's go-ahead.
   Confirm `craft status --remote` is clean, and for each `editPath`
   tracker confirm the target is a stored field so the control is
   editable, not silently read-only.

## Best practices

1. **Read the schema first** — bind `$state` paths to real field names;
   a typo'd path renders as empty, not as an error.
2. **Wrap optional sections in `visible`** — empty fields should collapse,
   not leave blank headings behind. (`ImageHeader` is the exception: its
   gradient fallback looks good without a guard.)
3. **Consider `Tabs` for complex layouts** — when a type has several
   substantial, distinct information groups or would become a long vertical
   stack, tabs keep the detail view clean and scannable. Prefer two to five
   clear, task-oriented tabs; keep always-needed identity/summary content
   outside the tabs, and keep a short layout visible instead of hiding it
   behind unnecessary navigation. Define exactly one child panel per tab in
   the same order, use unique `value` strings, and set `defaultValue` to
   one of them.
4. **ImageHeader for the hero** — title/subtitle overlay, graceful without an
   image.
5. **StatGrid for numeric clusters**, **DerivedStatGrid** when a computed field
   pairs with its base stat.
6. **BadgeList for string arrays**, `Checklist` for objective/task arrays,
   `Markdown` for long prose fields.
7. **Readable element keys** — "hero", "stats-section", "backstory-text";
   they are your only handle when editing later.
8. **Every child key defined, every element has `props`** — the two most
   common silent breakages.
9. **Keep schema and layout reference declarations together** — a
   `Select`/`BadgeList`/`FileReferenceGrid` with `fileTypeSlug` should
   always sit on a schema field with `referencedFileTypeSlug`.
10. **Match the house style** — check the other type files in
   `.craft/file-types/` before inventing a different structure.
