# Atlas Engine — Editor Design

The Editor is a Blender-style, dockable authoring environment. It is a standalone
application — not a mode of the client — built as a superset of the runtime.

## Editor Principles

- Dockable panels (recursive dock nodes with horizontal/vertical splits)
- Context-sensitive tools
- Data-driven editors
- Non-modal workflows
- Everything live-editable
- Undo/redo is core, not optional (CommandHistory implemented)

## Implementation Status

### Implemented ✅

- **UI Framework** — EditorPanel base class, DockNode, EditorLayout manager, LayoutPersistence
- **Console Panel** — Command parsing (`spawn_entity`, `ecs.dump`, `set tickrate`, `net.mode`, `help`)
- **World Graph Panel** — Node palette, canvas, inspector, and profiler sections
- **Project Picker Panel** — Project selection with recent projects list
- **Voice Command Panel** — Voice command processing and intent dispatch
- **Interaction Debugger Panel** — Logs interactions (speaker, text, intent, confidence)
- **Net Inspector Panel** — Network state debugging (mode, peers, RTT, bandwidth)
- **Game Packager Panel** — Build wired to `GamePackager::Package()` with `AssetCooker` + `BuildProfile`
- **Editor Assistant** — Router with LLM backend integration, falls back to hardcoded responses
- **ECS Inspector Panel** — Entity list with component types, summary view, mutation tracking
- **Asset Browser Panel** — Registry scanning, filtering, sorting, selection with UIDrawList rendering
- **Profiler Panel** — Frame timing history, system metrics, average/peak
- **Replay Timeline Panel** — Markers, divergence detection, frame branching, input viewer
- **State Hash Diff Panel** — Hash comparison, component breakdown, divergence detail
- **Job Trace Panel** — Execution order, determinism checking, mismatch highlighting
- **Save File Browser Panel** — Directory scanning, file metadata, selection
- **Proof Viewer Panel** — TLA+ syntax highlighting, tokenizer
- **CI Dashboard Panel** — Pipeline runs, check results, pass/fail tracking
- **Mesh Viewer Panel** — Mesh loading, view modes, vertex selection, bounds
- **Material Editor Panel** — Material parameters, preview mode, dirty tracking
- **Prefab Editor Panel** — Entity hierarchy, components, drag-and-drop composition
- **Quest Editor Panel** — Node graph, connections, preview mode, export/import
- **Inventory Editor Panel** — Items, filtering, sorting, export/import
- **AI Debugger Panel** — AI diagnostics, severity filtering, fix application, LLM integration
- **AI Diff Viewer Panel** — Hunk accept/reject workflow
- **Atlas Assistant Panel** — Prompt/suggestion UI framework
- **Desync Visualizer Panel** — Live desync event display with field-level detail
- **Game Mechanics UI Panel** — Elements, preview, export/import
- **Interaction Debug Panel** — UI interaction debugging
- **Tile Palette Panel** — Tile selection and painting
- **Rule Graph Editor Panel** — Rule graph editing with UIDrawList rendering
- **Truth UI Panel** — Aggregated simulation control, hash inspection, replay timeline, job trace
- **Play-In-Editor** — Simulate, Pause, Step, Possess Entity, Client-Server Loopback, State Restore
- **Tile Editor Module** — Paint, Erase, Select, LayerEdit, RuleEdit modes with brush/rect/flood fill
- **AI Aggregator** — Multi-backend routing, best-response selection
- **Template AI Backend** — Offline template-based responses
- **Asset Graph Assistant** — LLM-powered suggestions, explanations, mutations
- **Layout Persistence** — Save/restore panel arrangement to/from JSON
- **Launcher Screen** — Project scanning, selection, and confirmation

## Editor Capabilities

- Panel docking and layout management with persistence
- Console command execution
- World graph visualization and editing
- Project loading, switching, and launcher screen
- Voice command testing
- Interaction debugging
- Network state monitoring
- Live simulation preview (Play-In-Editor)
- Graph-based mechanics authoring
- AI-assisted content generation (with LLM backend support)
- Hot reload visualization
- Determinism debugging (state hash diff, job trace, desync visualization)
- Replay inspection and timeline scrubbing
- Asset browsing and mesh/material/prefab editing
- Quest and inventory editing
- Tile painting with brush, rectangle, and flood fill modes
- Game packaging with asset cooking and build profiles

## Default Layout

```
┌────────────────────────────────────┐
│ Top Bar (Mode / Play / Net)        │
├──────────────┬─────────────────────┤
│              │                     │
│  Tool Shelf  │   3D / Scene View   │
│              │                     │
├──────────────┼─────────────────────┤
│ Properties   │ Outliner / ECS Tree │
├──────────────┴─────────────────────┤
│ Console / Timeline / Node Graph    │
└────────────────────────────────────┘
```

Every panel is an Atlas panel.
Everything can be rearranged and saved.

## Core Panels

| Panel | Status | Description |
|-------|--------|-------------|
| Console | ✅ Implemented | Command-line automation with entity spawning |
| World Graph | ✅ Implemented | Node palette, canvas, inspector |
| Project Picker | ✅ Implemented | Project selection and recent list |
| Voice Commands | ✅ Implemented | Voice testing and intent dispatch |
| Interaction Debugger | ✅ Implemented | Interaction logging and analysis |
| Net Inspector | ✅ Implemented | Network topology, peers, RTT |
| Game Packager | ✅ Implemented | Build wired to AssetCooker + BuildProfile |
| ECS Inspector | ✅ Implemented | Entity list, components, mutation tracking |
| Asset Browser | ✅ Implemented | Registry scanning, filtering, sorting |
| Profiler | ✅ Implemented | Frame timing, system metrics, average/peak |
| Replay Timeline | ✅ Implemented | Markers, divergence, frame branching |
| State Hash Diff | ✅ Implemented | Hash comparison, divergence detail |
| Job Trace | ✅ Implemented | Execution order, determinism checking |
| Save File Browser | ✅ Implemented | Directory scanning, file metadata |
| Proof Viewer | ✅ Implemented | TLA+ syntax highlighting, tokenizer |
| CI Dashboard | ✅ Implemented | Pipeline runs, check results |
| Mesh Viewer | ✅ Implemented | 3D preview, wireframe, vertex selection |
| Material Editor | ✅ Implemented | Shader parameters, preview, dirty tracking |
| Prefab Editor | ✅ Implemented | Entity hierarchy, components, composition |
| Quest Editor | ✅ Implemented | Node graph, connections, preview |
| Inventory Editor | ✅ Implemented | Items, filtering, sorting |
| AI Debugger | ✅ Implemented | AI diagnostics, severity filtering, LLM |
| AI Diff Viewer | ✅ Implemented | Hunk accept/reject workflow |
| Atlas Assistant | ✅ Implemented | Prompt/suggestion UI framework |
| Desync Visualizer | ✅ Implemented | Live desync events with field detail |
| Game Mechanics UI | ✅ Implemented | Elements, preview, export/import |
| Tile Palette | ✅ Implemented | Tile selection and painting |
| Rule Graph Editor | ✅ Implemented | Rule graph editing |
| Truth UI | ✅ Implemented | Aggregated sim control and debugging |

## Play-In-Editor Modes

- ✅ Simulate (no player)
- ✅ Possess Entity
- ✅ Client-Server Loopback
- ✅ Pause / Step / State Restore

## Extended Design

For expanded editor UI design including Unreal-grade aesthetics, editor shell architecture,
editor attach protocol, permission tiers, headless server GUI, and self-hosting, see
[13_EDITOR_UI.md](13_EDITOR_UI.md).

For the underlying custom GUI system (DSL, layout solver, widget system, replay), see
[12_GUI_SYSTEM.md](12_GUI_SYSTEM.md).

For game UI authoring within the editor (inventory, HUD, menus), see
[18_GAME_GUI_AUTHORING.md](18_GAME_GUI_AUTHORING.md).
