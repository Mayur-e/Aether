---
name: craft-refactoring-file-types
description: "Read this BEFORE auditing, reviewing, improving, or cleaning up schemas in .craft/file-types/ — \"audit my file types\", \"improve my schemas\", \"review this file type\", \"clean up/modernize this schema\" — or when a schema shows bad patterns: duplicated derived fields, numbered slots, per-level or per-class branches, resource counters, copied reference properties, presentation strings built by expressions, or placeholder computed fields."
---

# Auditing & Improving File Types (workspace)

You are auditing a live Craft data model, not reformatting schemas. Find
where the model fights the platform — derived values stored by hand, rules
hardcoded where data belongs, presentation computed instead of laid out —
and fix it while preserving user-authored choices and mutable state.
Reusable rules move to their owning file types, derived values become
computed expression fields, and visual formatting moves to the layout.

## Critical rules

- **Findings first, pitched for a non-technical user.** Report what the
  audit found before changing anything — but assume the user builds
  worlds, not schemas. Lead with outcomes ("We can take your character
  sheet from 120 fields to 68, and armor class will update by itself"),
  keep each finding to a sentence or two of plain language, and put no
  code in the report — no schema JSON, no expression snippets, no
  backticked field names, no field tables; describe fields in words.
  Recommend one plan; the technical decisions are yours.
- **NEVER write computed values into data files or add expression fields to
  `required`.**
- **DO NOT preserve every existing field out of caution.** Delete
  presentation and alias computed fields; move reusable rules to referenced
  definitions; keep only genuine computed projections.
- **DO NOT branch on display names** such as `class.name == "Monk"`. Use
  stable keys, references, enums, or definition rows.
- **NEVER read target-file properties off a reference value.** A stored
  reference is an identity handle, not the target file's content: a direct
  read like `$self.field.prop` validates cleanly but is always `null` at
  runtime. Cross-file reads go through `load($self.field)?.prop`. Check
  every expression you touch, especially inside conditionals.
- **Preserve real user data.** Notes, choices, references, current values, and
  other irreproducible state need an explicit old-to-new mapping.
- Expression fields are top-level only. Compute a whole array/object instead
  of placing expressions inside rows.
- Cross-file reads through `load()` are one level deep.
- The main agent owns `craft pull`, type pushes, conflict resolution, and the
  final integrated verification. Do not let parallel workers race those
  operations in one workspace.

Read `.craft/instructions/references-and-computed-fields.md` for the
expression syntax, builtins, hydrated reference rules, and editable
computed-array pattern. Read `.craft/instructions/designing-layouts.md`
before judging the layout — the audit covers the layout too, and that doc
carries the full component catalog plus the "Auditing an existing layout"
workflow (binding walk, interactivity check, structure check) this topic's
layout table summarizes. For a
migration across many files, also read `.craft/instructions/bulk-work.md`
before loading the dataset.

## Classify every field first

| Kind | Meaning | Destination |
| --- | --- | --- |
| Stored state | A choice or mutable fact that cannot be reproduced | Stored file content |
| Referenced definition | Reusable rules or content shared by many files | The class, item, feature, species, or other owning file |
| Computed projection | A deterministic value derived from state and definitions | A top-level expression field |
| Presentation | Labels, colors, icons, dot strings, visibility, grouping | The layout |

Ask: **If this value disappeared, could it be reproduced from other canonical
data?** If yes, it should usually not be stored.

## Audit checklist: bad pattern → good pattern

| When you see | Replace it with |
| --- | --- |
| A derived value stored (or hand-entered) in content | A top-level expression field |
| Nested level ternaries or hardcoded threshold tables in an expression | Real progression rows on the file that owns the rule |
| `thing1`, `thing2`, `thing3` | An array with `maxItems` when entries share one role |
| Many meaningful equipment slots | One object keyed by stable slot names |
| The same cross-file read repeated for every slot or row | A collection plus `map`, `sum`, `values`, or `entries` and `load()` |
| One `used`, `max`, `remaining`, and dots field per resource | One stored `resourceState` map plus one computed `resources` array |
| A stored flag, computed modifier, and computed display repeated per skill, save, or stat | One stored map keyed by stable slugs plus one computed array |
| One collection per level or category | One stored collection plus a computed grouping |
| An expression building dots, badges, CSS classes, or prose | Typed data rendered by the layout |
| Copied fields such as `className` and `hitDieLabel` | Remove the copy or use one cohesive details object |
| A manual `{ name, referenceId }` object | A normal schema-declared reference field |
| An array of `{ item: <reference> }` with no row metadata | A direct array of references |
| Several target types with different bonus names | A shared contract such as `modifiers` on every target |
| A parent schema with branches for every class/subclass | Definition data on those referenced files |
| Computation declared through unrecognized legacy keys instead of `expression` | A normally typed top-level field with `expression` |

## Audit the layout too

The layout is part of the data model's user experience — a schema cleanup
that leaves a clumsy layout behind is half an audit. Check the type's
layout against the same standard and include layout upgrades in the
findings, framed as outcomes ("spell slots become tappable dots"):

| When the layout has | Replace it with |
| --- | --- |
| Numbered copies of the same element block (slot 1, slot 2, …) | One repeat over the array or computed collection |
| A computed display string (checkbox glyphs, dot strings, labels) bound to `Text` | The typed field bound to the matching interactive component (`Toggle`, `Dots`, `TrackBoxes`) |
| A plain number shown for a mutable counter (HP, uses, ammo) | `Stepper`, `Dots`, or `Progress` with `stepper` so play-time edits are one tap |
| A string-array of conditions or tags rendered as text | `ConditionToggles` or `BadgeList` |
| Reference arrays rendered as text or a plain table | `FileReferenceGrid` |
| Ability scores and their modifiers laid out by hand | `DerivedStatGrid` |
| Bindings to fields the audit deletes or renames | Bindings to the surviving typed fields |

Suggest, do not gold-plate: propose the component that fits the field's
role, keep the layout's existing structure and style choices, and fold
layout changes into the same approval and push batch as the schema
changes.

## Silent breakages to check

These compute wrong values without any visible error — hunt for them
explicitly:

- **Placeholder computed fields.** A field whose expression is just `null`,
  or a quoted "Could not migrate…" message, computes no real value; it marks
  logic that could not be carried forward automatically. The message form
  embeds the original source — treat that as the spec, together with the
  field's name, description, and layout usage, and rebuild it as data-driven
  rows on a referenced file plus a real expression — or remove the field
  with approval.
- **Unhydrated reference values.** Stored references should be hydrated
  objects (`{ slug, referenceId, fileTypeSlug }`). A raw slug string means
  `load()` resolves nothing and every dependent computed value silently
  degrades to its null default. Fix by rewriting the same slug through a
  normal content write and pushing (storage hydrates it) — do this BEFORE
  judging any expression that depends on the field.
- **Declared type contradicting the computed shape.** A field typed
  `"string"` whose expression yields numbers (or any type/shape mismatch)
  computes `null` at read time. Fix the declared type to the actual shape.
- **A collapse ternary feeding arithmetic.** An expression that reduces a
  mapped collection through `length == 1 ? ….at(0) : length == 0 ? null :
  …` yields the whole array once the collection has two or more entries,
  so arithmetic on the result silently computes null or zero. Rewrite as a
  per-row aggregate: `sum((rows ?? []).map(row => …))`.
- **Direct property reads off reference values** (`$self.field.prop`) —
  always `null`; rewrite as `load($self.field)?.prop`.

## Core patterns

### Progression rows, not source-code tables

Store level progressions as real arrays on the class, subclass, resource, or
system file that owns the rule. Select the last applicable row:

```json
{
  "proficiencyBonus": {
    "type": "number",
    "expression": "(load($self.class)?.proficiencyByLevel ?? []).filter(row => (row.level ?? 0) <= ($self.level ?? 1)).at(-1)?.bonus ?? 2"
  }
}
```

Use this for spell slots, resource maxima, experience thresholds, dice sizes,
and feature unlocks. Adding a new class should add definition data, not
branches to the character schema.

### Definitions plus mutable resource state

Classes and subclasses own resource rows with stable camelCase `key` values,
names, and `maxByLevel` data. The character stores only mutable state:

```json
{
  "resourceState": {
    "rage": { "current": 1 },
    "spellSlot1": { "current": 3 }
  }
}
```

One computed `resources` array combines the definitions, applies level
progression, and reads
`$self.resourceState?.[row.key]?.current ?? 0`. The layout repeats over that
array and writes through:

```json
"editPath": { "value": "/resourceState/{key}/current" }
```

When consolidating a legacy `used` counter into the standard `current`
state, define the transformation explicitly—often `current = maximum -
used`—and test it at the character's current level.

### Collapse per-item field families into a map plus a computed array

The same idea applies beyond resources. A field family repeated once per
skill, save, or stat — a stored `intimidationProficient`, a computed
`intimidationModifier` re-pasting the proficiency chain, a computed
`intimidationProficientDisplay` emitting "☑"/"☐", times eighteen skills —
is one concept modeled fifty-four times. Store one map keyed by stable
slugs and compute one array:

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

Skill definitions (stable `key`, display `name`, governing `ability`)
are rows on the rules or system file when one exists; for a fixed
vocabulary with no owning file, an inline literal array of the same rows
is acceptable. Computed fields can read earlier computed fields, so the
proficiency-bonus rule exists exactly once. The layout renders one row
repeated over `/skills`: a `Toggle` checkbox with
`"editPath": { "value": "/skillProficiencies/{key}" }` beside the
computed modifier. Delete every per-skill field; the display strings were
read-only text, and the toggle the user gets instead is tappable.

### Collections instead of enumerated fields

Use an array when entries share a role:

```text
portentDie1, portentDie2, portentDie3 → portentDice[]
spells1, spells2, ...                 → knownSpells[]
item1, item2, item3                   → items[] with maxItems: 3
```

Use an object for distinct slots such as armor, main hand, and off hand.
Aggregate those slots without repeating each name:

```js
($self.baseSpeed ?? 0) +
sum(values($self.equipment ?? {}), item =>
  load(item)?.modifiers?.speed ?? 0
)
```

Choose the collection shape from the domain, not merely to shorten the
expression.

### Referenced types own reusable rules

- Classes own hit dice, proficiencies, spell progression, resources, and
  class feature definitions.
- Subclasses own subclass resources and feature definitions.
- Species own movement, senses, traits, and modifiers.
- Equipment owns equip-slot metadata and a common modifier shape.
- Features and abilities own action types, descriptions, minimum levels, and
  resource definitions.

The character stores references and personal state. Computed fields load the
definitions and derive typed projections. Logic uses stable keys or reference
identity, never display names.

### References use the schema-native shape

Declare a reference directly:

```json
{ "boat": { "type": "string", "referencedFileTypeSlug": "boat" } }
```

Do not model hydrated storage with nested `name` or `referenceId` fields.
Craft hydrates slugs and expressions pass the whole value to `load()`.

Use a direct array of references unless a row has real metadata such as
quantity, status, notes, or an equipped flag.

## Workspace audit workflow

1. **Pull and inspect before judging.**
   - Start from a clean `craft status`, then `craft pull`.
   - Read `.craft/file-types/<slug>.json` and all related target type schemas.
   - Count local files of the affected type exactly with a shell command
     (e.g. a find/glob piped to a line count) — never estimate, and never
     report a number you did not count.
   - Read 1–3 representative files before deciding the target shape — check
     stored reference values are hydrated objects while you are there.
   - Spot-check nontrivial expressions against those files' stored data by
     evaluating them by hand (totals, weights, modifiers), including at
     least one file where a collection the expression aggregates has two
     or more entries. A result that does not add up is a broken
     expression: report it as a breakage, not a style issue.
   - For a large dataset, follow `.craft/instructions/bulk-work.md`.

2. **Inventory and classify.**
   Work through the audit checklist and the silent-breakage list. Group
   numbered fields, `used/max/remaining/display` families, per-level
   fields, copied reference properties, name-based branches, repeated
   cross-file reads, presentation strings, and manual reference objects.
   Sweep the layout against the layout table in the same pass — numbered
   element copies, display-string bindings, mutable counters without an
   interactive component.

3. **Design definitions first.**
   Establish shared contracts such as `modifiers`, `resources`,
   `maxByLevel`, and feature rows on the owning types. Confirm every property
   read after `load()` exists on each declared target.

4. **Design minimal stored state.**
   Keep canonical inputs, references, user selections, and state maps. Write
   an explicit old-to-new mapping for renamed or consolidated stored fields.

5. **Report findings, then edit schema, data, and layout locally.**
   - Present the findings before editing as a short, plain-language summary:
     what is broken or clumsy today and what each fix delivers ("spell save
     DC will calculate itself", "this trims the sheet from 120 fields to
     68"). Keep the technical plan — schema JSON, old-to-new mappings,
     pattern names — to yourself; recommend one plan rather than asking the
     user to pick between technical options or decide what to defer.
   - Migrate every affected file from the mapping; never copy computed or
     presentation-only values. Computed values are never stored in content,
     so a change that only adds, removes, or rewrites computed fields
     usually has nothing to migrate — confirm against representative files.
   - Replace layout bindings to removed aliases and display strings.
   - Add repeats and `editPath` for editable computed collections.
   - Keep all changes local until the full batch is internally consistent.

6. **Validate before pushing.**

   ```
   craft check
   craft status
   craft diff
   ```

   Fix every validation failure. Review warnings for stale or dangling
   references and computed values accidentally present in stored files.

7. **Push one coordinated batch.**

   Before pushing, show the user what is about to land in plain language:
   what will improve and the number of affected files — not raw diffs or
   schema JSON (share those only if asked). Ask one question: whether to go
   ahead. Wait for the go-ahead — local edits are free to iterate on; the
   push is the moment the changes reach the live project.

   ```
   craft push --include-types -m "audit <type>: <summary of improvements>"
   ```

   Type and content changes are evaluated against post-batch truth in the
   default atomic push. Do not use `--partial` for a coupled schema/data
   migration.

8. **Verify behavior.**
   Run `craft check` again and confirm `craft status --remote` is clean. Test
   minimum, middle, and maximum levels; empty collections; missing references;
   class/equipment switches; stale resource keys; level decreases; and every
   editable computed tracker.
   Verify values, not plausibility: for each rewritten expression, pick a
   file that exercises it and compare the computed value against a
   hand-computed expected result from the old behavior — each branch of a
   conditional needs its own file. Then confirm the pushed schema
   matches the approved plan: every field slated for deletion is gone and no
   constraint was added beyond the plan.

## Final checklist

- Every field is classified.
- No derived value is stored or required.
- No presentation logic lives in an expression.
- Progressions are structured data.
- Repeated fields use an appropriate array or keyed object.
- Shared target types expose consistent fields for `load()` reads.
- Logic uses stable keys or reference identity.
- Resource definitions and mutable state are separate.
- References are declared directly, and stored reference values are hydrated.
- Every expression has a declared output type that matches its computed shape,
  with intentional null defaults.
- Cross-file computation stays one level deep.
- Target-file reads go through `load()` — no property reads off a raw
  reference value.
- No placeholder (`null`) expressions and no legacy computed declarations
  remain.
- Existing content and layout bindings have an explicit migration path.
- The layout was audited too: repeats over collections, interactive
  components (`Dots`, `Stepper`, `Toggle`, `TrackBoxes`,
  `ConditionToggles`) for mutable state, no display-string bindings.
- Computed values were compared against hand-computed expected results,
  covering each conditional branch.
- The final schema matches the approved plan — deletions applied, nothing
  added beyond it.
- The findings report was outcome-focused plain language with one
  go-ahead question — no schema JSON, mappings, or technical menus.
- The user saw the plain-language summary and file count and approved the
  push.
