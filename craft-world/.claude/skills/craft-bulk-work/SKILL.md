---
name: craft-bulk-work
description: "Read this BEFORE creating a new project, designing a game system, creating many files, or starting any other large batch — covers the main-agent plan, capable subagent selection, dependency waves, nested delegation, and centralized verification and sync ownership (CLI push or platform import)."
---

# Planning and Delegating Large Builds

Large builds should be orchestrated, not written serially. When a task creates
an entire project, defines a game system, or produces many files, the main
agent owns the architecture and integration while subagents produce bounded,
parallel batches.

Use capable writing/reasoning subagents for real content. Sonnet-class workers
are the normal choice for well-specified file batches; use Opus-class workers
for system design, subtle cross-file continuity, or assignments that still
require substantial judgment. Avoid lightweight workers for work where weak
prose or inconsistent mechanics would become project data.

For a small edit or a handful of tightly coupled files, work directly. The
coordination overhead is not worth it.

## The main-agent flow

### 1. Design the project before delegating

The main agent first decides and writes down the shared foundation:

1. The user's intent, scope, audience, and explicit omissions.
2. Project settings: name, description, tags, page copy, art direction, GM
   persona, and whether the result needs images, maps, or playability.
3. A concise project overview/design brief: premise, tone, canon, naming
   conventions, and quality bar.
4. The game system or content model: core mechanics, vocabulary, scale, and
   invariants that every file must obey.
5. The folder structure and file-type inventory, including schemas, layouts,
   designations, and reference fields.
6. The exact file roster by type and folder, with target counts and the
   cross-type relationships that need to exist.
7. Dependency waves: which reference targets must exist before dependent
   records, examples, GM instructions, maps, and playability metadata.

Do not ask workers to invent these decisions independently. Put the shared
brief and roster in one workspace document they can all read, or include the
same authoritative context in every assignment.

### 2. Build and validate the foundation

The main agent creates or clones the project, reads the shipped instructions,
then authors project settings and file types before the content fan-out. Push
and validate schemas/layouts first (or, without the CLI, settle them locally
before any worker starts). Workers should receive settled file-type
slugs, schemas, paths, and naming conventions; parallelizing against a moving
schema creates rework and incompatible files.

### 3. Fan out by coherent ownership

Spawn workers by file type or cohesive domain — characters, locations, items,
rules, factions — rather than one worker per file. Give every worker:

- the absolute workspace path and shared design brief;
- its exact, non-overlapping paths and expected file count;
- the schemas and instruction topics it must read;
- tone, mechanics, naming, and reference conventions;
- the other planned files it may reference, including their stable slugs;
- explicit verification and reporting expectations.

Each worker owns only its assigned paths. It may run read-only checks such as
`craft status` and `craft diff`, but the main agent owns project settings,
file types, `pull`, `push`, `mv`, conflict resolution, and integration —
and, when working without the CLI, assembling the final workspace zip for
platform import. Concurrent sync mutations in one workspace race on shared
state.

When one worker's assignment is itself large and the agent platform supports
nested delegation, that worker should split its roster into non-overlapping
sub-batches and spawn its own capable subagents. The parent worker remains
responsible for reconciling tone, mechanics, references, and exact counts
across the whole file type before reporting completion.

### 4. Work in dependency waves

Parallelize independent work, not unresolved dependencies. A useful default
order is:

1. Project settings, overview, system design, folder plan, schemas, and layouts.
2. Foundational reference targets and rules content.
3. Dependent content files, parallelized by type or domain.
4. Cross-link repair, examples, GM instructions, and game-start content after
   names and paths are stable.
5. Images, maps, flows, and playability metadata after the content they attach
   to exists.

Wait for every worker in a wave, reconcile the results, then move to the next.
Never let two workers edit the same file or make one guess an output another
worker has not produced yet.

### 5. Integrate centrally and verify

The main agent reviews each completed wave for duplicate names, missing files,
invalid references, tone drift, and mechanical contradictions. Push quiet-tree
batches with scoped pathspecs where practical, verify the push, and only then
start work that depends on it.

At the end, reconcile the full roster and target counts, validate every type,
check cross-links and project/system consistency, and make sure
`craft status --remote` is clean. Run `craft check` and fix every
failure — those are push rejections waiting to happen. For a whole-project
build, also run `craft project-completeness`; accept its warnings only
when they describe an intentional scope choice that you report to the user.

Working without the CLI, the platform import's dry-run preview is the final
gate: after your own roster/consistency review, hand the user the zip and
tell them what the preview should show (counts by type, new vs updated) so
surprises surface before they apply it.
