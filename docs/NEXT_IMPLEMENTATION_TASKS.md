# Atlas Next Implementation Tasks

This document tracks the remaining implementation tasks to complete the vision outlined in the gaps analysis.

## Status: Current Implementation State

### ✅ Completed (High Priority)
- [x] ATLAS_CORE_CONTRACT.md documentation
- [x] Compile-time enforcement headers (AtlasContract.h, DeterministicRNG.h, SimulationGuard.h)
- [x] Runtime simulation guard implementation
- [x] CI determinism gate workflow
- [x] Contract violation bot
- [x] Contract scanner tool
- [x] Formal World State model (STATE_MODEL.md)
- [x] Time model specification (TIME_MODEL.md)
- [x] Asset importing foundation (AssetImporter.h, MarketplaceImporter.h)
- [x] Hash-based replay system
- [x] TLA+ formal specifications
- [x] Editor panel infrastructure (14+ panels)
- [x] State hash diff visualization
- [x] Replay timeline panel
- [x] ECS inspector
- [x] Network inspector
- [x] Project scaffolder (tools/create_atlas_project.py)
- [x] Determinism lint rules (tools/determinism_rules.yaml)
- [x] Public SDK documentation (docs/ATLAS_SDK.md)
- [x] CI failure playbook (docs/CI_FAILURE_PLAYBOOK.md)

### 🚧 In Progress (Medium Priority)

#### 1. Marketplace Integration Completion
**Status**: Foundation exists, needs full implementation

**Remaining work**:
- [x] Complete ItchIOImporter implementation
  - API endpoint integration
  - Asset metadata parsing
  - Download and cache management
- [x] Complete UnrealMarketplaceImporter implementation
  - .uasset format parsing/conversion (binary header parsing with magic number validation)
  - API credential management
  - Mesh conversion pipeline
- [x] Complete UnityAssetStoreImporter implementation
  - .prefab format parsing (YAML key extraction)
  - API credential management
  - Material and shader translation
- [x] Add comprehensive tests for each importer
- [x] Document marketplace API setup in MARKETPLACE_IMPORTING.md
**Files modified**:
- `engine/assets/MarketplaceImporter.h` — Added ParseUAssetHeader, ParsePrefabHeader, SetApiCredential
- `engine/assets/MarketplaceImporter.cpp` — Real format conversion implementations
- `tests/test_next_tasks_phase11.cpp` — 8 marketplace tests

#### 2. Editor Truth UI Enhancements
**Status**: Core panels exist, missing real-time features

**Remaining work**:
- [x] Add live simulation state inspection
  - Real-time component value viewer
  - Entity hierarchy tree view
  - Component mutation tracking
- [x] Enhance Determinism View panel
  - Per-system hash breakdown
  - Frame-by-frame hash ladder
  - Visual diff of divergent components
- [x] Add Replay Inspector features
  - Input frame viewer
  - Event timeline visualization
  - Branch point markers
- [x] Implement tick-step debugging
  - Step forward/backward
  - Breakpoint on hash mismatch
  - State snapshot comparison

**Files modified**:
- `editor/panels/ECSInspectorPanel.h/.cpp` — Component value inspection, entity hierarchy, mutation tracking
- `editor/panels/StateHashDiffPanel.h/.cpp` — Per-system hash breakdown, hash ladder visualization
- `engine/sim/TickStepDebugger.h/.cpp` — New tick-step debugger with breakpoints

#### 3. CI Enhancement
**Status**: Basic determinism gate exists, needs tooling

**Remaining work**:
- [x] Implement replay minimizer
  - Automatically reduce failing replays to minimal reproduction
  - Binary search through replay frames
  - Output minimal .atlasreplay file
- [x] Add crash report bundling
  - Capture state snapshot on crash
  - Bundle with replay up to crash point
  - Generate hash ladder for debugging
- [x] Add cross-platform hash comparison dashboard
  - GitHub Action artifact summary
  - Visual diff of platform results
  - Automated bisection hints

**Files created**:
- `tools/replay_minimizer.py`
- `tools/crash_reporter.py`
- `.github/workflows/atlas_crash_reporter.yml`

### 📋 Planned (Lower Priority)

#### 4. CORE/ Namespace Enforcement
**Status**: Directory structure exists, CMake enforcement implemented

**Remaining work**:
- [x] Add CMake target dependency validation
  - Enforce that `core/` never depends on other modules
  - Enforce that `simulation/` only depends on `core/`
  - Enforce that `editor/` never links `simulation` directly
- [x] Add compile-time include guards
  - Prevent `simulation` from including `presentation` headers
  - Prevent `editor` from including `sim` internals
- [x] Document dependency graph in ARCHITECTURE.md

**Files created/modified**:
- `engine/core/contract/IncludeFirewall.h` - Compile-time include guards
- `docs/ARCHITECTURE.md` - Added layer enforcement documentation

#### 5. Advanced Debugging Tools
**Status**: Core features implemented

**Remaining work**:
- [x] State hash diff visualizer (interactive tool)
  - Component-by-component comparison
  - Per-system hash breakdown
  - Frame-by-frame hash ladder visualization
- [x] Tick-step debugger
  - Step forward/backward/jump
  - Tick and hash-mismatch breakpoints
  - Pause/resume with callbacks
- [x] Job execution tracer
  - Per-frame job execution order
  - Timing analysis
  - Determinism verification
- [x] Replay divergence inspector
  - Automatic divergence point detection
  - Component diff at divergence
  - Input replay from divergence
- [x] Standalone state diff viewer tool
  - `tools/state_diff_viewer.py` - Interactive CLI
- [x] Standalone replay inspector tool
  - `tools/replay_inspector.py` - Interactive CLI

**Files created**:
- `engine/sim/TickStepDebugger.h/.cpp` — Tick-step debugger with breakpoints
- `engine/render/VulkanRenderer.h/.cpp` — Sync primitives and memory pools

#### 6. Asset Pipeline Enhancements
**Status**: Basic pipeline exists, needs polish

**Remaining work**:
- [x] Hot-reload for marketplace assets
  - Detect asset updates from marketplace
  - Automatic re-import
  - Version conflict resolution
- [x] Asset validation dashboard
  - Integrity check results
  - Dependency graph visualization
  - Hash verification status
- [x] Mod asset sandboxing
  - Deterministic hash verification
  - Permission model enforcement
  - CPU/memory budget tracking

**Files created/modified**:
- `engine/assets/MarketplaceImporter.h` - Added MarketplaceHotReloader, AssetValidationDashboard, ModAssetSandbox
- `engine/assets/MarketplaceImporter.cpp` - Full implementations
- `editor/panels/ReplayTimelinePanel.h/.cpp` - Input frame viewer, event timeline, branch points
- `tools/state_diff_viewer.py` - Standalone state diff CLI
- `tools/replay_inspector.py` - Standalone replay inspector CLI
- `tests/test_next_tasks_phase12.cpp` - 25 new tests

#### 7. GL Viewport, Layout Resize, Bitmap Font
**Status**: Complete

**Completed work**:
- [x] GLViewportFramebuffer implementation
  - FBO with color texture and depth renderbuffer
  - Runtime GL extension resolution
  - Graceful fallback on platforms without GL
- [x] Editor layout resize propagation
  - `UIScreen::ScaleLayout` for proportional widget scaling
  - `UIManager::SetViewportSize` triggers layout scaling
  - `Engine::ProcessWindowEvents` propagates resize to UI
  - `Engine::InitEditor` sets initial viewport size
- [x] Bitmap font rendering in GLRenderer
  - Built-in 5×7 pixel font for printable ASCII (0x20–0x7E)
  - Replaces solid-rectangle placeholder text

**Files created/modified**:
- `engine/render/GLViewportFramebuffer.h` - GL FBO header
- `engine/render/GLViewportFramebuffer.cpp` - GL FBO implementation (conditional compilation)
- `engine/render/GLRenderer.cpp` - Bitmap font DrawText
- `engine/ui/UIScreenGraph.h/.cpp` - ScaleLayout method
- `engine/ui/UIManager.cpp` - SetViewportSize propagation
- `engine/core/Engine.cpp` - Resize and initial viewport wiring
- `tests/test_next_tasks_phase13.cpp` - 12 new tests

#### 8. Vulkan Device Abstraction, HttpLLMBackend, Font Discovery
**Status**: Complete

**Completed work**:
- [x] Vulkan hardware device abstraction
  - VkPhysicalDeviceInfo struct (device name, vendor, type, memory, capabilities)
  - VkQueueFamilyInfo struct (graphics, compute, transfer, present queues)
  - VkSwapChainDesc struct (image count, present mode, format)
  - VkDeviceConfig struct (application name, validation layers, extensions)
  - InitDevice/ShutdownDevice lifecycle with simulated GPU
  - Queue family discovery (graphics, compute, dedicated transfer)
  - Swap chain create/resize with validation
  - Device enumeration and selection
- [x] HttpLLMBackend for external LLM API integration
  - OpenAI-compatible chat completions API
  - API key management and authentication headers
  - JSON request body construction with proper escaping
  - JSON response parsing for content extraction
  - Error handling (no client, no key, HTTP error, parse error)
  - Success/failure tracking metrics
  - LLMBackendRegistry integration
  - Configurable timeout
- [x] FontBootstrap font discovery enhancements
  - Multiple font search path registration with deduplication
  - Font file discovery (.ttf, .otf) across all search paths
  - Direct font loading by path with TTF header validation
  - Loaded font path tracking
  - Init() auto-registers asset fonts directory

**Files created/modified**:
- `engine/render/VulkanRenderer.h` - VkPhysicalDeviceInfo, VkQueueFamilyInfo, VkSwapChainDesc, VkDeviceConfig, device management API
- `engine/render/VulkanRenderer.cpp` - Device initialization, queue discovery, swap chain management stubs
- `engine/ai/LLMBackend.h` - HttpLLMBackend class declaration
- `engine/ai/LLMBackend.cpp` - HttpLLMBackend implementation with JSON request/response handling
- `engine/ui/FontBootstrap.h` - Font search path and discovery API
- `engine/ui/FontBootstrap.cpp` - Font discovery and loading implementations
- `tests/test_next_tasks_phase14.cpp` - 35 new tests

#### 9. MeshViewerPanel and PrefabEditorPanel Draw Implementations
**Status**: Complete

**Completed work**:
- [x] MeshViewerPanel::Draw() implementation
  - Title bar with current view mode label (Solid, Wireframe, Solid+Wire, Normals)
  - Vertex/triangle count and bounding box display
  - Viewport area with border
  - Grid overlay (horizontal and vertical crosshair lines, togglable)
  - Normals overlay indicator when enabled
  - Selected vertex indicator
  - "No mesh loaded" fallback text
- [x] PrefabEditorPanel::Draw() implementation
  - Title bar with dirty indicator (*)
  - Entity hierarchy tree with recursive parent-child rendering
  - Indented child entities with depth-based offset
  - Parent nodes marked with > prefix
  - Component inspector section for selected entity
  - Component type display with property key-value pairs
  - "(no components)" fallback for entities without components
  - "Empty prefab" fallback text

**Files created/modified**:
- `editor/panels/MeshViewerPanel.h` - Added UIDrawList include, m_drawList member, GetDrawList() accessor, moved Draw() to .cpp
- `editor/panels/MeshViewerPanel.cpp` - New — full Draw() implementation with UIDrawList commands
- `editor/panels/PrefabEditorPanel.h` - Added UIDrawList include, m_drawList member, GetDrawList() accessor, DrawEntityRow() helper, moved Draw() to .cpp
- `editor/panels/PrefabEditorPanel.cpp` - New — full Draw() implementation with hierarchy tree and component inspector
- `tests/test_next_tasks_phase15.cpp` - 15 new tests

## Implementation Priority Order

1. **Marketplace Integration Completion** (Highest business value)
   - Enables asset ecosystem integration
   - Differentiates Atlas from other engines
   
2. **Editor Truth UI Enhancements** (Highest developer value)
   - Critical for debugging determinism issues
   - Improves developer experience significantly

3. **CI Enhancement** (Highest quality value)
   - Reduces debugging time
   - Catches issues earlier

4. **CORE/ Namespace Enforcement** (Architectural integrity)
   - Prevents technical debt
   - Enforces clean architecture

5. **Advanced Debugging Tools** (Long-term value)
   - Nice-to-have features
   - Can be added incrementally

6. **Asset Pipeline Enhancements** (Polish)
   - Quality-of-life improvements
   - Can be deferred

## Next Steps

1. ~~Connect Vulkan hardware device — wire stub pipeline to real VkDevice/VkCommandBuffer (requires Vulkan SDK)~~ ✅ Conditional Vulkan SDK integration added (ATLAS_HAS_VULKAN_SDK)
2. ~~Ship real font — bundle Inter-Regular.ttf in builds~~ ✅ Inter-Regular.ttf bundled in assets/fonts/
3. ~~Deploy HttpLLMBackend — configure with production API endpoint and model~~ ✅ LLMBackendFactory added for env-based and explicit configuration

#### 10. Networking Improvements
**Status**: Complete

**Completed work**:
- [x] Latency/jitter simulation (GetSimulatedLatencyMs with deterministic jitter)
- [x] Bandwidth enforcement on Send/Broadcast (CanSendBytes gate, drop counting)
- [x] CRC32 packet checksum validation (compute on send, verify on receive)
- [x] Manual replication frequency (TriggerManualReplication for on-demand sync)
- [x] Reliable/unreliable delta split (CollectDelta for reliable, CollectUnreliableDelta for unreliable)
- [x] Hardening integration with NetContext (SetHardening, stats recording)
- [x] 23 new tests covering all networking improvements

**Files modified**:
- `engine/net/NetHardening.h/.cpp` — Added GetSimulatedLatencyMs()
- `engine/net/NetContext.h/.cpp` — Added SetHardening, ComputeChecksum, ValidateChecksum, bandwidth enforcement
- `engine/net/Replication.h/.cpp` — Added TriggerManualReplication, CollectUnreliableDelta, reliable/unreliable callbacks
- `tests/test_net_improvements.cpp` — 23 new tests

#### 11. AI Assistant LLM Wiring + Permission Enforcement
**Status**: Complete

**Completed work**:
- [x] EditorAssistant wired to LLMBackendRegistry
  - SetLLMBackend() / GetLLMBackend() methods
  - Unknown intents forwarded to LLM backend when available
  - Falls back to hardcoded responses when no LLM configured
  - Known intents (ExplainPerformance, ExplainGraphNode) unchanged
- [x] AssetGraphAssistant wired to LLMBackendRegistry
  - SetLLMBackend() / GetLLMBackend() methods
  - SuggestNodes() uses LLM for context-aware suggestions
  - ExplainGraph() uses LLM for richer explanations
  - MutateGraph() uses LLM for AI-powered mutation descriptions
  - All methods fall back to template responses when no LLM
- [x] EditorAttachProtocol permission enforcement
  - RequestOperation() validates connection state + permission tier + mode restrictions
  - Replay mode: read-only (blocks ModifyState, InjectInput, EditAssets, RunCI)
  - HeadlessServer mode: blocks StepSimulation
  - Permission tier check via IsOperationAllowed()
- [x] 21 new tests covering all changes

**Files modified**:
- `editor/assistant/EditorAssistant.h/.cpp` — Added LLM backend integration
- `editor/assistant/AssetGraphAssistant.h/.cpp` — Added LLM backend integration
- `editor/ui/EditorAttachProtocol.h/.cpp` — Added RequestOperation() with mode-aware enforcement
- `tests/test_next_tasks_phase16.cpp` — 21 new tests

#### 12. Remaining Panel Draw() Implementations
**Status**: Complete

**Completed work**:
- [x] AssetBrowserPanel Draw() implementation
  - Title bar, summary line with count/sort mode/filter
  - Asset list with extension display
  - Selected asset highlighting
- [x] NetInspectorPanel Draw() implementation
  - Title bar, network mode, peer count, connected count, average RTT
  - Peer list with per-peer status (connected/disconnected) and RTT
- [x] JobTracePanel inherits EditorPanel + Draw() implementation
  - Title bar, execution order status (consistent/mismatch)
  - Column header and trace entry rows
  - Mismatch highlighting in red
- [x] StateHashDiffPanel inherits EditorPanel + Draw() implementation
  - Title bar, divergence status summary
  - Column header and hash entry rows
  - Divergent entries highlighted in red
  - Per-system breakdown section when available
- [x] 26 new tests covering all 4 panel Draw() implementations

**Files modified**:
- `editor/panels/AssetBrowserPanel.h/.cpp` — Added UIDrawList member, GetDrawList(), full Draw() rendering
- `editor/panels/NetInspectorPanel.h/.cpp` — Added UIDrawList member, GetDrawList(), full Draw() rendering
- `editor/panels/JobTracePanel.h/.cpp` — Added EditorPanel inheritance, UIDrawList, Name(), Draw()
- `editor/panels/StateHashDiffPanel.h/.cpp` — Added EditorPanel inheritance, UIDrawList, Name(), Draw()
- `tests/test_panel_draw_impl.cpp` — 26 new tests

#### 13. Hot Reload World Migration (Component Remap)
**Status**: Complete

**Completed work**:
- [x] ComponentSchema and FieldDesc definitions
  - Stable field IDs, type-safe field descriptors
  - Schema version tracking per component type
- [x] RemapComponent function
  - Field-by-field migration from old to new schema
  - Matching fields copied, new fields zero-initialized, removed fields dropped
  - Type and null-data safety checks
- [x] ComponentMigrationManager
  - Multi-version schema registry
  - MigrateToLatest for automatic version upgrade
  - IsHotReloadSafe check (blocks type-changing field migrations)
  - NeedsMigration detection
- [x] 10 tests covering all migration scenarios

**Files created**:
- `engine/sim/ComponentMigration.h` — ComponentSchema, FieldDesc, RemapComponent, ComponentMigrationManager
- `engine/sim/ComponentMigration.cpp` — Full implementations

#### 14. Server/Client Simulation Mirror Tool
**Status**: Complete

**Completed work**:
- [x] ISimulation abstract interface (Step, WorldHash, CurrentTick)
- [x] SimMirrorController
  - Lockstep execution of two simulations with same inputs
  - Per-tick hash comparison for determinism verification
  - MirrorDesyncEvent recording with tick/hash details
  - RunFrames batch execution with early exit on desync
  - Desync callback for live notification
  - Enable/disable toggle for mirror mode
  - Reset for clearing desync history
- [x] 8 tests covering determinism, desync detection, callbacks

**Files created**:
- `engine/sim/SimMirror.h` — ISimulation, MirrorDesyncEvent, SimMirrorController
- `engine/sim/SimMirror.cpp` — Full implementations

#### 15. Live Net-Desync Visualizer
**Status**: Complete

**Completed work**:
- [x] DesyncVisualizerPanel (EditorPanel subclass)
  - Live desync event display with tick, server hash, client hash
  - Per-field detail view (entity, component, field, server/client values)
  - Event selection and highlighting
  - Automatic sync from SimMirrorController
  - UIDrawList rendering with color-coded entries
- [x] 8 tests covering empty state, events, field details, mirror sync

**Files created**:
- `editor/panels/DesyncVisualizerPanel.h` — DesyncFieldDetail, DesyncDisplayEvent, DesyncVisualizerPanel
- `editor/panels/DesyncVisualizerPanel.cpp` — Full Draw() implementation

#### 17. CI Headless Editor Build + Replay Tests
**Status**: Complete

**Completed work**:
- [x] GitHub Actions workflow for headless builds
  - Build all targets (Editor, Server, Client, Runtime, Tests)
  - Binary existence verification
  - Full test suite execution
  - Headless editor and server smoke tests
  - Golden replay determinism verification (double-run hash comparison)
  - Contract scanner integration

**Files created**:
- `.github/workflows/atlas_headless_editor.yml` — Full CI workflow

#### 18. GUI Interaction Hardening + Tile Painting Modes
**Status**: Complete

**Completed work**:
- [x] Mouse button remapping to 0-indexed in X11Window and Win32Window
  - All UI interaction managers check `mouseButton == 0` for left-click — now matches
  - Scroll wheel (X11 buttons 4/5) emits `ScrollWheel` events correctly
  - `TextInput` synthesized from `KeyDown` for printable characters
- [x] ODR violation fixes
  - `atlas::ai::AIResponse` vs `AggregatorResponse` rename in AIAggregator.h
  - `atlas::editor::InteractionLogEntry` vs `DebuggerLogEntry` rename in InteractionDebugger.h
- [x] Widget ordering determinism
  - `UIScreen::GetChildren` now returns children sorted by widget ID
- [x] InputFieldManager cursor fix
  - `SetText` now advances cursor to end of new text
- [x] Tile painting modes (brush size, rectangle fill, flood fill)
  - `BrushPaint` / `BrushErase` — configurable half-radius, Square and Circle shapes
  - `PaintRect` / `EraseRect` — axis-aligned rectangle fill/erase with corner normalisation
  - `FloodFill` — 4-connected BFS flood fill respecting original tile identity, safety cap at 1M cells
  - `SetOnTilePainted` / `SetOnTileErased` — event callbacks for undo/redo integration
- [x] 19 new tile painting tests

**Files modified**:
- `engine/render/X11Window.cpp` — TranslateButton() normalization, ScrollWheel events
- `engine/render/Win32Window.cpp` — WM_LBUTTONDOWN/MBUTTONDOWN/RBUTTONDOWN → 0/1/2
- `engine/core/Engine.cpp` — TextInput synthesis from KeyDown, ScrollWheel forwarding
- `engine/ui/UIScreen.cpp` — Sorted GetChildren()
- `engine/ui/InputFieldManager.cpp` — Cursor-to-end on SetText
- `engine/ai/AIAggregator.h` — AggregatorResponse rename
- `editor/ai/InteractionDebugger.h` — DebuggerLogEntry rename
- `editor/tools/TileEditorModule.h` — BrushShape, TilePaintEvent, brush/rect/flood API
- `editor/tools/TileEditorModule.cpp` — Full implementations
- `tests/test_tile_editor.cpp` — 17 new tile painting mode tests

#### 19. AI Procedural Content Generator + AI Debugger Panel + Hot Reload Config
**Status**: Complete

**Completed work**:
- [x] ProceduralGenerator framework (engine/ai/ProceduralGenerator.h/.cpp)
  - Culture-aware name generation (NPC, place, item) for 6 cultures (Generic, Nordic, Eastern, Latin, SciFi, Fantasy)
  - Biome asset list generation for 9 biome types (Forest, Desert, Mountain, Ocean, Tundra, Swamp, Plains, Volcanic, Urban)
  - Quest template generation for 6 quest types (Fetch, Combat, Exploration, Escort, Puzzle, Delivery) with difficulty scaling
  - Shader suggestions per biome/environment type
  - Deterministic xorshift64 RNG with optional LLM backend for richer output
- [x] AIDebuggerPanel (editor/panels/AIDebuggerPanel.h/.cpp)
  - Live AI diagnostic display with severity filtering (Info, Warning, Error, Critical)
  - Fix application tracking with callback support
  - LLM backend connection status
  - Auto-diagnose capability via LLM or template fallback
  - UIDrawList rendering with color-coded severity indicators
- [x] HotReloadConfig (engine/sim/HotReloadConfig.h/.cpp)
  - Typed config parameters (int32, uint32, float, double, bool, string)
  - Tick-gated changes (no mid-tick mutation)
  - Parameter locking
  - Version tracking (per-param and global)
  - Change callbacks with old/new value reporting
  - Reset-to-default support
- [x] 35 new tests covering all three systems

**Files created**:
- `engine/ai/ProceduralGenerator.h` — Framework header
- `engine/ai/ProceduralGenerator.cpp` — Full implementation
- `editor/panels/AIDebuggerPanel.h` — Panel header
- `editor/panels/AIDebuggerPanel.cpp` — Panel implementation
- `engine/sim/HotReloadConfig.h` — Config header
- `engine/sim/HotReloadConfig.cpp` — Config implementation
- `tests/test_next_tasks_phase19.cpp` — 35 new tests

#### 20. Renderer Backend Abstraction & Capabilities
**Status**: Complete

**Completed work**:
- [x] Extended RenderAPI enum with None, DX11, and Null variants
- [x] RendererCapabilities struct for centralized GPU capability reporting
  - Bindless textures, compute shaders, ray tracing flags
  - Max MSAA samples, HDR swapchain, max texture size, max uniform buffers
  - Device name and driver version strings
- [x] RendererBackend abstract base class
  - Init/Shutdown lifecycle
  - BeginFrame/EndFrame cycle
  - SetViewport
  - GetAPI() and GetCapabilities() accessors
- [x] NullRendererBackend for headless/CI operation
  - Zero-capability initialization
  - Frame counting and viewport tracking
  - Silent operation with no GPU resources
- [x] RendererFactory for API-driven backend instantiation
  - Creates NullRendererBackend for RenderAPI::Null
  - Returns nullptr for GPU backends when platform libraries unavailable
  - Returns nullptr for RenderAPI::None
- [x] UIBackend interface for rendering-backend-agnostic UI drawing
  - BeginFrame/Draw/EndFrame lifecycle
  - NullUIBackend with frame and draw call counting
- [x] 16 new tests covering all components

**Files created**:
- `engine/render/RendererCapabilities.h` — GPU capability struct
- `engine/render/RendererBackend.h` — Abstract backend base class
- `engine/render/NullRendererBackend.h` — Null backend header
- `engine/render/NullRendererBackend.cpp` — Null backend implementation
- `engine/render/RendererFactory.h` — Factory header
- `engine/render/RendererFactory.cpp` — Factory implementation
- `engine/ui/UIBackend.h` — UI backend interface + NullUIBackend
- `tests/test_renderer_backend.cpp` — 16 new tests

**Files modified**:
- `engine/render/RenderAPI.h` — Added None, DX11, Null enum values
- `engine/CMakeLists.txt` — Added new source files

#### 21. Project Scaffolder + SDK Docs + CI Playbook + Determinism Rules
**Status**: Complete

**Completed work**:
- [x] `tools/create_atlas_project.py` — Constitution-compliant project scaffolder
  - Creates `.atlas` manifest with `determinism_profile` and `binary_abi`
  - Generates stub system with `ATLAS_SYSTEM_TRAITS` declaration
  - Generates stub component with `SchemaVersion`
  - Creates `replays/`, `migrations/`, `scripts/`, `assets/`, `config/` directories
  - Rejects existing directories
- [x] `tools/determinism_rules.yaml` — Machine-readable determinism lint rules
  - 8 rule categories (containers, time, RNG, floating-point, threading, IO, SIMD, UI libs)
  - Simulation directory scope and exemption lists
  - Consumable by `contract_scan.py` and CI
- [x] `docs/ATLAS_SDK.md` — Public SDK documentation
  - Project creation workflows (scaffolder + atlas_init)
  - System rules, determinism rules, scripting, replays, ABI, CI, distribution
  - Tool reference table
- [x] `docs/CI_FAILURE_PLAYBOOK.md` — CI failure troubleshooting guide
  - Fix procedures for: determinism lint, replay hash mismatch, contract violations,
    ABI failures, cross-platform divergence, build failures, test failures, crash reports
  - Tool references and general debugging workflow

**Files created**:
- `tools/create_atlas_project.py` — Project scaffolder
- `tools/determinism_rules.yaml` — Lint rule definitions
- `docs/ATLAS_SDK.md` — SDK documentation
- `docs/CI_FAILURE_PLAYBOOK.md` — CI failure playbook
- `tests/test_create_atlas_project.py` — 7 tests for scaffolder and rules

#### 22. Deterministic Scripting VM
**Status**: Complete

**Completed work**:
- [x] Stack-based bytecode VM (ScriptVM) with 24 opcodes
  - Arithmetic: ADD, SUB, MUL, DIV, MOD, NEG
  - Comparison: EQ, NEQ, LT, GT, LTE, GTE
  - Logic: AND, OR, NOT
  - Stack: PUSH_INT, PUSH_FLOAT, PUSH_STRING
  - Variables: LOAD_VAR, STORE_VAR
  - Control flow: CALL, JMP, JMP_IF, RET, HALT
- [x] Budget enforcement (configurable max steps, budget exceeded detection)
- [x] Native function registration for sandboxed API
- [x] Deterministic state hashing for replay safety
- [x] ScriptSystem for tick-integrated execution
  - Registration-order execution (deterministic)
  - atlas_tick and atlas_seed variable injection
  - Contract validation (deterministic, replay-safe)
  - Combined hash across all scripts
- [x] ScriptSandbox with 8 safe built-in functions
  - atlas_abs, atlas_min, atlas_max, atlas_clamp
  - atlas_floor, atlas_ceil, atlas_sqrt, atlas_strlen
- [x] 28 tests for VM, system, and sandbox

**Files created**:
- `engine/script/ScriptVM.h` — VM header with opcodes, values, instructions
- `engine/script/ScriptVM.cpp` — Full opcode implementation
- `engine/script/ScriptSystem.h` — Tick-integrated script manager
- `engine/script/ScriptSystem.cpp` — Registration and execution
- `engine/script/ScriptSandbox.h` — Safe builtin declarations
- `engine/script/ScriptSandbox.cpp` — Builtin implementations
- `docs/19_SCRIPTING_VM.md` — Documentation

#### 23. Binary Compatibility Layer (ABI Capsules)
**Status**: Complete

**Completed work**:
- [x] ABIVersion with string parsing ("atlas_abi_v1_0") and compatibility checks
- [x] ABIFunctionTable with 8 simulation-safe C ABI entry points
  - RegisterSystem, StepSimulation, GetCurrentTick
  - GetComponent, SetComponent
  - SubmitInput, EmitEvent, GetWorldHash
  - Completeness and bound-count queries
- [x] ABICapsule with version, description, seal, and readiness tracking
- [x] ABIRegistry for capsule discovery and project binding
  - Exact version lookup
  - Compatible version resolution (same major, highest minor ≤ requested)
  - Project bind/unbind/query
- [x] 20 tests for capsule, version, registry, and project binding

**Files created**:
- `engine/abi/ABICapsule.h` — Version, function table, capsule
- `engine/abi/ABICapsule.cpp` — Full implementation
- `engine/abi/ABIRegistry.h` — Registry header
- `engine/abi/ABIRegistry.cpp` — Discovery and binding
- `docs/21_BINARY_COMPATIBILITY.md` — Documentation

### Phase 21: Cleanup & Alignment (Code Quality)

**Goal:** Remove accumulated tech-debt, eliminate spaghetti code patterns, and
align the project structure for long-term maintainability.

- [x] Move root-level clutter files to `legacy/` directory
  - `gaps.txt`, `gui21.txt`, `gui_issues.txt`, `implement please`, `projectupdate`, `aiimp`
  - `From old repo/` directory
- [x] Eliminate unsafe `static_cast` downcasts in `Engine::ProcessWindowEvents`
  - Added virtual `SetViewport()` to `UIRenderer` base class
  - `GLRenderer` and `VulkanRenderer` now `override` the virtual method
- [x] Extract duplicated code in `Engine::RunEditor` / `Engine::RunClient`
  - New `Engine::TickSimulation()` — pure simulation tick (no UI)
  - New `Engine::UpdateUI()` — UI context update (separate from sim tick)
  - New `Engine::RenderFrame()` — shared render + swap logic
- [x] Separate UI update from simulation tick callback
  - Previously UI was updated inside `m_scheduler.Tick()` lambda
  - Now simulation and presentation are clearly separated
- [x] Add Phase 21 test file (`test_next_tasks_phase21.cpp`)

**Files changed:**
- `engine/ui/UIRenderer.h` — Added virtual `SetViewport` to base class
- `engine/render/GLRenderer.h` — `SetViewport` marked `override`
- `engine/render/VulkanRenderer.h` — `SetViewport` marked `override`
- `engine/core/Engine.h` — Added `TickSimulation`, `UpdateUI`, `RenderFrame` helpers
- `engine/core/Engine.cpp` — Refactored RunEditor/RunClient, removed downcasts
- Root cleanup — Moved 6 files + 1 directory to `legacy/`

#### 24. RunServer Tick Deduplication & Shutdown Fix
**Status**: Complete

**Completed work**:
- [x] Extract `StepSimulationTick()` private helper from `TickSimulation()`
  - Contains the shared simulation step: time advance, ECS update, physics step,
    flow graph execution, game module tick
  - `TickSimulation()` now calls `StepSimulationTick()` inside the scheduler callback
  - `RunServer()` calls `StepSimulationTick()` followed by server-specific snapshot logic
  - Eliminates ~20 duplicated lines between `TickSimulation()` and `RunServer()`
- [x] Fix `Engine::Shutdown()` guard
  - Replaced `if (m_running)` guard with `if (!m_shutdown)` using a dedicated `m_shutdown` flag
  - Ensures explicit cleanup (logging, net/physics shutdown) runs even after
    a normal game loop exit where `m_running` is already false
  - Prevents double-shutdown on repeated calls
- [x] Clean up `runtime/main.cpp`
  - Removed leftover `makeModuleContext` lambda pattern
  - Constructs `GameModuleContext` directly, matching client/server entry points
- [x] 3 new tests (shutdown after run, editor physics tick, client module tick)

**Files modified**:
- `engine/core/Engine.h` — Added `StepSimulationTick()` declaration, `m_shutdown` flag
- `engine/core/Engine.cpp` — Extracted `StepSimulationTick()`, refactored `RunServer()`, fixed `Shutdown()`
- `runtime/main.cpp` — Removed `makeModuleContext` lambda
- `tests/test_engine_full_integration.cpp` — 3 new tests
- `tests/main.cpp` — Registered new tests

## References

- Original gaps analysis: `legacy/gaps.txt`
- Asset importing plan: `legacy/implement please`
- Project structure plan: `legacy/projectupdate`
- Core contract: `docs/ATLAS_CORE_CONTRACT.md`
- Determinism enforcement: `docs/ATLAS_DETERMINISM_ENFORCEMENT.md`
