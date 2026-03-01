# NovaForge Refactoring Plan

> **Status**: In Progress  
> **Priority**: Critical — must be completed before any new features  
> **Created**: 2026-03-01  
> **Tracking PR**: #300

This document captures every code-quality issue found during the full-repository
audit and organises them into actionable phases.  Each item includes the files
affected, a brief description of the problem, and the fix.

---

## Table of Contents

1. [Phase 1 — Eliminate Code Duplication](#phase-1--eliminate-code-duplication)
2. [Phase 2 — Split Monolithic Files](#phase-2--split-monolithic-files)
3. [Phase 3 — Header Hygiene](#phase-3--header-hygiene)
4. [Phase 4 — Logging & Error Handling](#phase-4--logging--error-handling)
5. [Phase 5 — Magic Numbers & Constants](#phase-5--magic-numbers--constants)
6. [Phase 6 — Naming Consistency](#phase-6--naming-consistency)
7. [Phase 7 — Documentation Gaps](#phase-7--documentation-gaps)
8. [Phase 8 — Stubs & TODOs](#phase-8--stubs--todos)

---

## Phase 1 — Eliminate Code Duplication

### 1.1 Duplicated JSON helpers (CRITICAL)

Seven independent copies of lightweight JSON extraction functions exist:

| File | Functions |
|------|-----------|
| `cpp_server/src/game_session.cpp` | `extractJsonString`, `extractJsonFloat` |
| `cpp_server/src/data/ship_database.cpp` | `extractString`, `extractFloat`, `extractInt` |
| `cpp_server/src/data/npc_database.cpp` | `extractString`, `extractFloat`, `extractInt` |
| `cpp_server/src/data/wormhole_database.cpp` | `extractString`, `extractFloat`, `extractInt` |
| `cpp_server/src/data/world_persistence.cpp` | `extractString`, `extractFloat`, `extractInt`, `extractDouble`, `extractBool`, `extractObject` |
| `editor/ui/EditorLayout.cpp` | `extractString`, `extractFloat`, `extractInt` |
| `cpp_client/src/ui/atlas/atlas_context.cpp` | `extractJsonString`, `extractJsonFloat` |

**Fix**: Create `cpp_server/include/utils/json_helpers.h` with a single
implementation.  Replace all call-sites.

### 1.2 Duplicated `escapeJson` / `escapeJsonString`

Static escape functions appear in `game_session.cpp` and
`world_persistence.cpp`.

**Fix**: Move into `json_helpers.h`.

### 1.3 Repeated entity-creation boilerplate

~60 instances of `make_unique<components::*>` sequences in
`game_session.cpp`, `world_persistence.cpp`, and 4+ system files follow
identical patterns for Position / Velocity / Health / Ship / Faction.

**Fix**: (Future) Create an `EntityBuilder` utility once the file splits are
stable.

---

## Phase 2 — Split Monolithic Files

All splits follow the pattern established in PR #299: extract domain-specific
`.cpp` files; keep the original as a thin orchestrator / aggregator.  Headers
stay unchanged (same class interface).

| File | Lines | Split Into |
|------|-------|-----------|
| `cpp_server/src/game_session.cpp` | 1 540 | `game_session_network.cpp`, `game_session_entities.cpp`, `game_session_combat.cpp`, `game_session_movement.cpp`, `game_session_stations.cpp`, `game_session_scanning.cpp`, `game_session_missions.cpp`, `game_session_utils.cpp` |
| `cpp_server/src/data/world_persistence.cpp` | 2 066 | `world_serializer.cpp`, `world_deserializer.cpp`, `persistence_json_utils.cpp`, `world_persistence_compressed.cpp` |
| `cpp_client/src/core/application.cpp` | 1 937 | `application_input.cpp`, `application_rendering.cpp`, `application_movement.cpp`, `application_state.cpp`, `application_entities.cpp` |
| `cpp_client/src/ui/atlas/atlas_widgets.cpp` | 2 467 | `atlas_widgets_panels.cpp`, `atlas_widgets_hud.cpp`, `atlas_widgets_lists.cpp`, `atlas_widgets_forms.cpp`, `atlas_widgets_sidebar.cpp`, `atlas_widgets_cards.cpp`, `atlas_widgets_effects.cpp`, `atlas_widgets_menus.cpp` |

Each new file includes only the headers it needs.  `CMakeLists.txt` is updated
to list the new source files.

---

## Phase 3 — Header Hygiene

### 3.1 Mixed guard styles

~170 files use `#pragma once`; ~250 files use `#ifndef`.  Within the
`#ifndef` group, macro names vary:

```
CONTEXT_MENU_H                         // short, no namespace prefix
NOVAFORGE_ECS_SYSTEM_H                 // full project prefix
NOVAFORGE_SYSTEMS_COMBAT_SYSTEM_H      // full path prefix
```

**Fix**: Standardize all new/modified headers to `#pragma once`.  Existing
`#ifndef` guards are left as-is to keep diffs minimal; they will be converted
opportunistically.

### 3.2 Include ordering

Several `cpp_server/systems/` headers place project includes before standard
library headers.

**Fix**: Enforce order — *own header → project headers → third-party → std* —
and document in `CODING_GUIDELINES.md`.

---

## Phase 4 — Logging & Error Handling

### 4.1 Three logging systems

| Module | Logger | Notes |
|--------|--------|-------|
| `engine/` | `atlas::Logger` (static, console only) | Minimal |
| `cpp_server/` | `atlas::Logger` singleton (thread-safe, file + console) | Full-featured |
| `cpp_client/` | `FileLogger` (stream-buffer redirect) | Indirect |

23 files still use raw `std::cout` / `std::cerr`.

**Fix**: In `cpp_server/`, replace all raw `std::cout` / `std::cerr` with the
server `Logger`.  Document the convention in guidelines.

### 4.2 Bare `catch (...)` blocks

10+ instances silently swallow exceptions (notably `world_persistence.cpp`,
`wormhole_database.cpp`, `ship_database.cpp`).

**Fix**: Add a `LOG_ERROR` line to every bare `catch (...)`.

---

## Phase 5 — Magic Numbers & Constants

Hardcoded gameplay values scattered across:

- `application.cpp` — player/NPC health, capacitor, spawn distances
- `station_renderer.cpp` — station part dimensions
- `ship_physics.cpp` — distance thresholds (2500 m)
- `radial_menu.cpp` — distance steps array
- `solar_system_scene.cpp` — planet radii

**Fix**: Extract to named `constexpr` constants at file / namespace scope.

---

## Phase 6 — Naming Consistency

| Category | Engine / Editor | cpp_server | cpp_client |
|----------|----------------|-----------|-----------|
| File names | PascalCase | snake_case | snake_case |
| Methods | PascalCase | snake_case | **mixed** |
| Members | `m_` prefix | `_` suffix | `m_` prefix |

**Fix**: Do **not** bulk-rename across modules (too disruptive).  Instead:

- Align *new code* in each module with that module's existing dominant style.
- Document the per-module convention in `CODING_GUIDELINES.md`.

---

## Phase 7 — Documentation Gaps

| Item | Status |
|------|--------|
| `CONTRIBUTING.md` references Python/pip instead of C++/CMake | Needs update |
| `cpp_client/include/` header doc coverage | ~28 % — needs `@brief` on public classes |
| No C++ coding style guide | Create `docs/CODING_GUIDELINES.md` |
| 60+ session-log files in `docs/sessions/` | Low priority — leave as-is |

---

## Phase 8 — Stubs & TODOs

| Location | TODO | Priority |
|----------|------|----------|
| `cpp_server/src/auth/steam_auth.cpp` (×6) | Unimplemented Steam authentication | Low (feature, not refactor) |
| `cpp_client/src/core/application.cpp:281` | World persistence integration | Medium |
| `cpp_client/src/core/application.cpp:505` | Drone commands | Low |
| `cpp_client/src/ui/atlas/atlas_title_screen.cpp` | Multiplayer lobby UI | Low |
| `cpp_client/src/network/entity_message_parser.cpp:84` | Interpolation delay | Medium |

**Fix**: Leave TODOs in place; they are tracked features, not refactoring
issues.

---

## Acceptance Criteria

- [ ] All 3 812 existing tests continue to pass after every phase
- [ ] No new files exceed 500 lines unless justified
- [ ] `CODING_GUIDELINES.md` merged and linked from `CONTRIBUTING.md`
- [ ] CMakeLists.txt updated for every new source file
