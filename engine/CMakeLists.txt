# Detect optional platform dependencies early
if(UNIX AND NOT APPLE)
    find_package(X11 QUIET)
    find_package(OpenGL QUIET)
    find_library(VULKAN_LIB vulkan)
    find_package(Vulkan QUIET)
endif()

add_library(AtlasEngine STATIC
    core/Engine.cpp
    core/Logger.cpp
    core/CrashHandler.cpp
    ecs/ECS.cpp
    graphvm/GraphVM.cpp
    graphvm/GraphCompiler.cpp
    graphvm/GraphSerializer.cpp
    graphvm/CollaborativeEditor.cpp
    assets/AssetRegistry.cpp
    assets/AssetBinary.cpp
    assets/AssetImporter.cpp
    assets/MarketplaceImporter.cpp
    assets/AssetValidator.cpp
    assets/HttpClient.cpp
    assets/SocketHttpClient.cpp
    net/NetContext.cpp
    net/NetHardening.cpp
    net/QoSScheduler.cpp
    net/Replication.cpp
    sim/TickScheduler.cpp
    world/CubeSphereLayout.cpp
    world/VoxelGridLayout.cpp
    world/TerrainMeshGenerator.cpp
    world/NoiseGenerator.cpp
    world/WorldStreamer.cpp
    world/GalaxyGenerator.cpp
    world/WorldGraph.cpp
    world/WorldNodes.cpp
    world/HeightfieldMesher.cpp
    project/ProjectManager.cpp
    command/CommandHistory.cpp
    interaction/Interaction.cpp
    voice/VoiceCommand.cpp
    plugin/PluginSystem.cpp
    strategygraph/StrategyGraph.cpp
    rules/ServerRules.cpp
    conversation/ConversationGraph.cpp
    ai/AISignalRegistry.cpp
    ai/AIMemory.cpp
    ai/RelationshipModel.cpp
    tile/TileGraph.cpp
    tile/TileNodes.cpp
    sound/SoundGraph.cpp
    sound/SoundNodes.cpp
    ai/BehaviorGraph.cpp
    ai/BehaviorNodes.cpp
    character/CharacterGraph.cpp
    character/CharacterNodes.cpp
    animation/AnimationGraph.cpp
    animation/AnimationNodes.cpp
    animation/DeterministicAnimationGraph.cpp
    weapon/WeaponGraph.cpp
    weapon/WeaponNodes.cpp
    ui/UIGraph.cpp
    ui/UINodes.cpp
    ui/UILogicGraph.cpp
    ui/UILogicNodes.cpp
    ui/UIScreenGraph.cpp
    ui/UICommandBus.cpp
    ui/GUIInputRecorder.cpp
    ui/GUIDSLParser.cpp
    ui/UIManager.cpp
    ui/UIRenderer.cpp
    ui/UILayoutSolver.cpp
    ui/HeadlessGUI.cpp
    ui/GameGUIAsset.cpp
    ui/WidgetDSL.cpp
    ui/GameGUIBinding.cpp
    ui/FontBootstrap.cpp
    ui/DiagnosticsOverlay.cpp
    ui/TextRenderer.cpp
    ui/UIEventRouter.cpp
    ui/MenuManager.cpp
    ui/TabManager.cpp
    ui/ScrollManager.cpp
    ui/ToolbarManager.cpp
    ui/FocusManager.cpp
    ui/TooltipManager.cpp
    ui/DockManager.cpp
    ui/InputFieldManager.cpp
    ui/SliderManager.cpp
    ui/ComboBoxManager.cpp
    ui/CheckboxManager.cpp
    ui/TreeNodeManager.cpp
    ui/SplitterManager.cpp
    ui/ColorPickerManager.cpp
    ui/UIDrawList.cpp
    ui/UIStyle.cpp
    ui/UISceneGraph.cpp
    tile/TileRenderer.cpp
    tile/TileChunkBuilder.cpp
    flow/GameFlowGraph.cpp
    flow/GameFlowNodes.cpp
    flow/FlowGraphIR.cpp
    flow/FlowGraphDebugger.cpp
    flow/FlowGraphRefactorer.cpp
    schema/SchemaValidator.cpp
    graphvm/GraphCache.cpp
    ai/FactionRouter.cpp
    ai/AtlasAICore.cpp
    ai/WebAggregationKB.cpp
    ai/ProjectContext.cpp
    ai/AIAssetDecisionFramework.cpp
    ai/ProceduralGenerator.cpp
    asset_graph/AssetGraphExecutor.cpp
    audio/AudioEngine.cpp
    camera/Camera.cpp
    gameplay/MechanicAsset.cpp
    gameplay/SkillTree.cpp
    input/InputManager.cpp
    interaction/InteractionSystem.cpp
    interaction/RuleIntentResolver.cpp
    mod/ModAssetRegistry.cpp
    mod/ModLoader.cpp
    physics/PhysicsWorld.cpp
    story/StoryGraph.cpp
    production/GamePackager.cpp
    production/AssetCooker.cpp
    production/BuildProfile.cpp
    production/PlatformTarget.cpp
    production/BuildManifest.cpp
    production/CertifiedBuild.cpp
    procedural/ProceduralMeshGraph.cpp
    procedural/ProceduralMeshNodes.cpp
    procedural/ProceduralMaterialGraph.cpp
    procedural/ProceduralMaterialNodes.cpp
    procedural/LODBakingGraph.cpp
    procedural/LODBakingNodes.cpp
    procedural/ShipHullGraph.cpp
    procedural/ShipHullNodes.cpp
    sim/ReplayProofExporter.cpp
    sim/ReplayRecorder.cpp
    sim/StateHasher.cpp
    sim/JobTracer.cpp
    sim/ReplayDivergenceInspector.cpp
    sim/FPDriftDetector.cpp
    sim/TimeModel.cpp
    sim/WorldState.cpp
    sim/SaveSystem.cpp
    core/DeterministicAllocator.cpp
    core/PermissionManager.cpp
    ui/HUDOverlay.cpp
    module/ModuleLoader.cpp
    render/VulkanRenderer.cpp
    render/AtlasShaderIR.cpp
    render/GLViewportFramebuffer.cpp
    render/NullRendererBackend.cpp
    render/RendererFactory.cpp
    sim/SimulationStateAuditor.cpp
    sim/DesyncReproducer.cpp
    sim/TLCModelChecker.cpp
    sim/DeterminismVersioning.cpp
    sim/WorldStateSerializer.cpp
    sim/ReplayVersioning.cpp
    sim/TickStepDebugger.cpp
    sim/ComponentMigration.cpp
    sim/HotReloadConfig.cpp
    sim/SimMirror.cpp
    assets/AssetCategoryRegistry.cpp
    assets/ServerAssetValidator.cpp
    ai/LLMBackend.cpp
    production/BuildAuditLog.cpp
    tools/ProjectNamespaceRewriter.cpp
    script/ScriptVM.cpp
    script/ScriptSystem.cpp
    script/ScriptSandbox.cpp
    abi/ABICapsule.cpp
    abi/ABIRegistry.cpp
)

# Conditionally add platform/render sources that require X11, Win32, or OpenGL
if(UNIX AND NOT APPLE)
    if(X11_FOUND)
        target_sources(AtlasEngine PRIVATE platform/X11Window.cpp)
    endif()
    if(OpenGL_FOUND)
        target_sources(AtlasEngine PRIVATE render/GLRenderer.cpp)
    endif()
elseif(WIN32)
    target_sources(AtlasEngine PRIVATE platform/Win32Window.cpp render/GLRenderer.cpp)
else()
    target_sources(AtlasEngine PRIVATE render/GLRenderer.cpp)
endif()

target_include_directories(AtlasEngine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/atlas>
)

# Floating-point consistency flags — prevent compiler optimizations that
# break determinism across platforms or compilation units.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(AtlasEngine PRIVATE
        -ffp-contract=off        # Disable FMA contraction
    )
elseif(MSVC)
    target_compile_options(AtlasEngine PRIVATE
        /fp:strict               # Strict floating-point model
    )
endif()

# Link dynamic loading library on Unix
if(UNIX AND NOT APPLE)
    target_link_libraries(AtlasEngine PUBLIC dl)
    if(X11_FOUND)
        target_link_libraries(AtlasEngine PUBLIC ${X11_LIBRARIES})
        target_include_directories(AtlasEngine PRIVATE ${X11_INCLUDE_DIR})
        target_compile_definitions(AtlasEngine PRIVATE ATLAS_HAS_X11)
    endif()
    if(OpenGL_FOUND)
        target_link_libraries(AtlasEngine PUBLIC OpenGL::GL)
    endif()
    if(VULKAN_LIB)
        target_link_libraries(AtlasEngine PUBLIC ${VULKAN_LIB})
    endif()
    if(Vulkan_FOUND)
        target_compile_definitions(AtlasEngine PUBLIC ATLAS_HAS_VULKAN_SDK)
        target_include_directories(AtlasEngine PUBLIC ${Vulkan_INCLUDE_DIRS})
    endif()
elseif(APPLE)
    target_link_libraries(AtlasEngine PUBLIC dl)
elseif(WIN32)
    target_compile_definitions(AtlasEngine PRIVATE ATLAS_HAS_WIN32)
    target_link_libraries(AtlasEngine PUBLIC opengl32 gdi32 user32)
endif()

# ============================================================
# Simulation / Render Include Firewall
# ============================================================
# AtlasSimulation is an INTERFACE library that exposes ONLY the
# simulation-safe source directories.  Downstream targets that
# link against AtlasSimulation are guaranteed not to pull in
# render, platform, or UI headers, enforcing the sim/render
# separation documented in docs/ATLAS_SIMULATION_PHILOSOPHY.md.
#
# Simulation-safe directories:
#   core, ecs, sim, net, graphvm, assets, world, ai, rules,
#   schema, flow, gameplay, mod, physics, story, command,
#   interaction, plugin, strategygraph, conversation, tile,
#   sound, character, animation, weapon, module, audio, input,
#   production, asset_graph
#
# Render / presentation directories (NOT included):
#   render, platform, ui, camera
# ============================================================
add_library(AtlasSimulation INTERFACE)
target_include_directories(AtlasSimulation INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ecs>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sim>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/net>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/graphvm>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/assets>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/world>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ai>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/rules>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/schema>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/flow>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gameplay>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mod>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/physics>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/story>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/command>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interaction>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/plugin>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/strategygraph>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/conversation>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tile>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sound>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/character>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/animation>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/weapon>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/module>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/audio>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/input>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/production>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/asset_graph>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/procedural>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/script>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/abi>
)

# --- Install / Export ---
install(TARGETS AtlasEngine
    EXPORT AtlasEngineTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Install public headers preserving directory structure
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/atlas
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT AtlasEngineTargets
    FILE AtlasEngineTargets.cmake
    NAMESPACE Atlas::
    DESTINATION lib/cmake/AtlasEngine
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/AtlasEngineConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/AtlasEngineConfig.cmake"
    INSTALL_DESTINATION lib/cmake/AtlasEngine
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/AtlasEngineConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/AtlasEngineConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/AtlasEngineConfigVersion.cmake"
    DESTINATION lib/cmake/AtlasEngine
)

# ============================================================
# Include Firewall Enforcement
# ============================================================
# Simulation code must never include render headers.
# This prevents accidental non-deterministic coupling.

# Collect simulation sources
file(GLOB_RECURSE SIM_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/sim/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sim/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/contract/*.h"
)

# Custom target that checks for forbidden includes
add_custom_target(atlas_include_firewall
    COMMAND ${CMAKE_COMMAND} -E echo "Checking include firewall..."
    COMMAND ${CMAKE_COMMAND}
        -DSIM_DIR=${CMAKE_CURRENT_SOURCE_DIR}/sim
        -DCONTRACT_DIR=${CMAKE_CURRENT_SOURCE_DIR}/core/contract
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/check_include_firewall.cmake
    COMMENT "Verifying simulation code does not include render headers"
    VERBATIM
)
