#pragma once
#include <string>
#include <functional>
#include "../core/Engine.h"
#include "../module/ModuleLoader.h"
#include "../module/IGameModule.h"
#include "../net/Replication.h"
#include "../rules/ServerRules.h"
#include "../assets/AssetRegistry.h"
#include "../project/ProjectManager.h"

namespace atlas::bootstrap {

/// Parsed command-line arguments common to all entry points.
struct CommandLineArgs {
    std::string projectPath;
    std::string modulePath;
    std::string modeStr = "client";
    bool validateOnly = false;
    bool showHelp = false;
};

/// Parse standard Atlas command-line arguments.
/// Recognises --project, --module, --mode, --validate-only, and --help.
CommandLineArgs ParseCommandLine(int argc, char* argv[]);

/// Configure an EngineConfig from a loaded project descriptor and engine mode.
/// Copies tickRate and assetRoot when a project is loaded.
void ApplyProjectConfig(EngineConfig& cfg, EngineMode mode);

/// Build a GameModuleContext from the running engine plus supporting objects.
module::GameModuleContext MakeModuleContext(
    Engine& engine,
    net::ReplicationManager& replication,
    asset::AssetRegistry& assets);

/// Full module lifecycle: load → RegisterTypes → ConfigureReplication →
/// ConfigureServerRules → OnStart.  Returns false on load failure.
/// Optionally skips ConfigureServerRules (e.g. in client mode).
bool LoadAndStartModule(
    module::ModuleLoader& loader,
    const std::string& modulePath,
    Engine& engine,
    net::ReplicationManager& replication,
    asset::AssetRegistry& assets,
    bool configureServerRules = true);

/// Shut down a loaded module cleanly.
void ShutdownModule(
    module::ModuleLoader& loader,
    Engine& engine,
    net::ReplicationManager& replication,
    asset::AssetRegistry& assets);

} // namespace atlas::bootstrap
