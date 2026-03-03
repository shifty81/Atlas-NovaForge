# Arena2D — Atlas Reference Project

Minimal 2D tile-based arena game demonstrating Atlas Engine capabilities.

## Purpose

This project serves as a **scalability proof** — it validates that Atlas can host
multiple games simultaneously without engine pollution.

## Project Structure

```
arena2d/
├── arena2d.atlas           # Project manifest
├── Plugin.toml             # Plugin descriptor
├── worlds/                 # TileGraph files
│   └── arena.tilegraph     # Arena map generation
├── ai/                     # BehaviorGraph files
│   └── enemy_patrol.behaviorgraph
├── data/                   # Game content (JSON)
│   └── enemies.json
├── config/                 # Runtime configuration
│   └── runtime.json
├── assets/                 # Art assets (sprites, audio)
└── README.md
```

## Key Features Demonstrated

- TileGraph-based procedural level generation
- BehaviorGraph-driven AI enemies
- JSON-defined game content
- Minimal runtime configuration
- Atlas project conventions compliance

## Validation

This project can be loaded alongside EVE Offline or any other Atlas project.
Deleting this project directory should have zero impact on the engine or other projects.
