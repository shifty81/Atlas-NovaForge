# Atlas Projects

This directory contains Atlas Engine game projects. Each project is a self-contained
directory with a `.atlas` manifest file that defines its structure.

## Available Projects

| Project | Description |
|---------|-------------|
| `eveoffline/` | PVE space simulator — the reference implementation |
| `arena2d/` | Minimal 2D arena — scalability reference |
| `atlas-sample/` | Minimal sample project for learning |

## Creating a New Project

1. Create a new directory under `projects/`
2. Add a `.atlas` manifest file (see `schemas/atlas.project.v1.json`)
3. Create required subdirectories: `worlds/`, `assets/`, `data/`
4. Add WorldGraph files to `worlds/`
5. Open the `.atlas` file in Atlas Editor

## Project Structure (Required)

```
my-project/
├── project.atlas          # Project manifest (required)
├── Plugin.toml            # Plugin descriptor (recommended)
├── README.md              # Project documentation (recommended)
├── worlds/                # WorldGraph files
├── strategy/              # StrategyGraph files (optional)
├── data/                  # Game content
├── assets/                # Models, textures, audio
├── config/                # Runtime configuration (optional)
├── Code/                  # Game-specific code (optional)
├── Schemas/               # Data validation schemas (optional)
├── Tests/                 # Project tests (optional)
└── ai/                    # AI rules and knowledge graphs (optional)
```

See [PROJECT_GUIDELINES.md](../docs/PROJECT_GUIDELINES.md) for complete rules.

## Portability — Drag and Drop to Atlas Repo

Projects in this directory are designed to be **portable** — you can copy any
project folder into the Atlas repo's `projects/` directory and it should work
as long as it follows the guidelines below.

### Portability Requirements

1. **Self-contained** — all project-specific files live inside the project folder
2. **Relative paths only** — the `.atlas` manifest must not use absolute paths
3. **No symlinks** — use real files/directories for cross-platform compatibility
4. **Standard structure** — must have `.atlas` manifest + `worlds/`, `assets/`, `data/`
5. **Plugin.toml** — recommended so the engine can register the project properly

### How to Port a Project

```bash
# Copy project folder to Atlas repo
cp -r projects/eveoffline /path/to/Atlas/projects/

# Validate it works
cd /path/to/Atlas
./scripts/validate_project.sh projects/eveoffline
```

### Shared Data

Some projects (like `eveoffline`) reference shared data at the repository root
via `../../data` in their `data/manifest.json`. When porting to another repo:

- Either copy the shared `data/` directory into the project's `data/` folder
- Or ensure the target repo has the same `data/` layout at its root

### Validate Before Porting

Run the portability validator before moving a project:

```bash
./scripts/validate_project.sh projects/eveoffline
```

This checks for all portability requirements and reports warnings for anything
that might need attention.

## Building Projects

Use the project build script to build engine, client, and server:

```bash
./scripts/build_project.sh                    # Build everything (Release)
./scripts/build_project.sh Release engine     # Build engine only
./scripts/build_project.sh Release client     # Build client only
./scripts/build_project.sh Release server     # Build server only
./scripts/build_project.sh Release test       # Build and run all tests
./scripts/build_project.sh Release validate   # Validate project structure
```

See `scripts/build_project.sh --help` or the script header for all targets.
