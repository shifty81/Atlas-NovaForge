# EVEOFFLINE — Atlas Project

This is the EVEOFFLINE game project structured as an Atlas Engine project.

## Opening in Atlas Editor

1. Launch Atlas Editor
2. Select `eveoffline.atlas` from the project picker
3. The editor will mount this directory and load all modules

## Project Structure

```
eveoffline/
├── eveoffline.atlas                    # Project manifest (atlas.project.v1)
├── Plugin.toml                         # Plugin descriptor for engine registration
├── worlds/                             # WorldGraph and TileGraph files
│   ├── galaxy.worldgraph               # Galaxy generation graph
│   └── station_interior.tilegraph      # Station interior tile generation
├── strategy/                           # StrategyGraph files
│   └── faction_ai.strategygraph        # Faction AI strategy graph
├── conversations/                      # Conversation graphs
│   └── captain_greeting.conversation   # Captain greeting dialogue
├── ai/                                 # AI configuration and behavior graphs
│   ├── npc_config.json                 # NPC AI signal and memory config
│   ├── combat.behaviorgraph            # NPC combat behavior graph
│   ├── trade.behaviorgraph             # NPC trade behavior graph
│   └── patrol.behaviorgraph            # NPC patrol behavior graph
├── data/                               # Game data manifest
│   └── manifest.json                   # References content at repo root data/
├── config/                             # Runtime configuration
│   └── runtime.json                    # Server rules, tick rate, network mode
├── Code/                               # Game-specific initialization code
│   ├── GameInit.h                      # Entry point header
│   ├── GameInit.cpp                    # Entry point implementation
│   └── Adapters/                       # Engine-game adapter layer
│       ├── ECSAdapter.h                # ECS component/system registration
│       └── ECSAdapter.cpp
├── Schemas/                            # Project-specific data schemas
│   ├── eveoffline.ship.v1.json         # Ship data validation schema
│   ├── eveoffline.module.v1.json       # Module data validation schema
│   └── eveoffline.npc.v1.json          # NPC data validation schema
├── Tests/                              # Project validation tests
│   └── test_project_structure.cpp      # Validates project completeness
├── assets/                             # Models, textures, audio
└── README.md
```

## Game Data

All game data files (76 JSON files) are **bundled inside this project** under the
`data/` directory. The project is fully self-contained — you can copy this entire
folder into any Atlas repo's `projects/` directory and it will work standalone.

The `data/manifest.json` uses `"contentRoot": "."` to resolve all paths locally
within the project folder.

The manifest organizes data into categories:

| Category             | Example files                                      |
|----------------------|----------------------------------------------------|
| Ships                | frigates.json, cruisers.json, capitals.json        |
| Modules              | weapons.json, defensive.json, drones.json          |
| Skills               | skills.json, science_skills.json                   |
| Universe             | systems.json, warp_mechanics.json                  |
| Fleet                | captain_backgrounds.json, chatter_templates.json   |
| Missions             | level1–5_missions.json, epic_arcs.json             |
| Market               | prices.json                                        |
| Industry             | blueprints.json, datacores.json                    |
| NPCs                 | pirates.json, sleepers.json, hireable_pilots.json   |
| Exploration          | sites.json, deadspace_complexes.json               |
| Corporations         | corporations.json                                  |
| Contracts            | contracts.json                                     |
| Character Creation   | races.json, clones.json, implants.json             |
| Wormholes            | wormhole_classes.json, wormhole_effects.json       |
| Asteroid Fields      | belts.json, ore_types.json                         |
| Harvesting           | gas_types.json, ice_types.json                     |
| Security             | concord_and_insurance.json                         |
| Planetary Interaction| resources.json                                     |
| UI                   | eve_dark_theme.json                                |

## World Graphs

- **galaxy.worldgraph** — Procedural galaxy generation using noise fields, region
  assignment, and stargate connectivity.
- **station_interior.tilegraph** — Tile-based generation for station interior layouts.

## Strategy Graphs

- **faction_ai.strategygraph** — Drives faction-level AI decisions including territory
  control, resource allocation, and diplomatic posture.

## Conversation Graphs

- **captain_greeting.conversation** — Sample dialogue tree for captain interactions,
  demonstrating Dialogue, PlayerChoice, and branching nodes.

## Behavior Graphs

- **combat.behaviorgraph** — NPC combat AI using threat assessment, utility scoring
  (threat weight 0.7, health weight 0.3), emotion updates, and action selection.
- **trade.behaviorgraph** — NPC trade AI with market assessment and profit-weighted
  utility scoring (threat weight 0.2, health/profit weight 0.8).
- **patrol.behaviorgraph** — NPC patrol AI with balanced threat/health scoring (0.5/0.5),
  emotion-driven confidence, and alert level output.

## AI Configuration

`ai/npc_config.json` defines:
- **Signal namespaces** — faction morale/hostility, sector tension/resources, economy
  supply/demand, pirate threat, fleet cohesion.
- **Memory config** — decay rate and maximum memories per NPC.
- **Behavior graphs** — references to combat, trade, and patrol behavior graphs.

## Runtime Configuration

`config/runtime.json` defines server rules with hot-reloadable parameters:
- `miningYieldMultiplier` — scales mining output (0.1–5.0×)
- `npcSpawnRate` — NPC spawn frequency (0–10×)
- `marketUpdateInterval` — seconds between market ticks (60–3600)
- `pirateAggressionLevel` — pirate hostility (0.0–1.0)

## Adding New Content

1. **New data files**: Add the JSON file under the appropriate `data/` subdirectory
   within this project folder, then add the filename to the relevant category in
   `data/manifest.json`.
2. **New world/tile graphs**: Place `.worldgraph` or `.tilegraph` files in `worlds/`.
3. **New strategy graphs**: Place `.strategygraph` files in `strategy/`.
4. **New conversations**: Place `.conversation` files in `conversations/`.
5. **New behavior graphs**: Place `.behaviorgraph` files in `ai/`.
6. **New server rules**: Add entries to `config/runtime.json` under `serverRules`.
7. **New data schemas**: Place `eveoffline.*.v1.json` files in `Schemas/`.
8. **New game code**: Add source files under `Code/` (entry point) or
   `Code/Adapters/` (engine-game bridges).

All game data is loaded by Atlas Engine via the project manifest.

## Validation

Run the project validation test to verify structure completeness:

```bash
g++ -std=c++17 Tests/test_project_structure.cpp -o test_project_structure -lstdc++fs
./test_project_structure .
```
