#include "RuntimeBootstrap.h"
#include "../core/Logger.h"
#include <iostream>

namespace atlas::bootstrap {

CommandLineArgs ParseCommandLine(int argc, char* argv[]) {
    CommandLineArgs args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--project" && i + 1 < argc) {
            args.projectPath = argv[++i];
        } else if (arg == "--module" && i + 1 < argc) {
            args.modulePath = argv[++i];
        } else if (arg == "--mode" && i + 1 < argc) {
            args.modeStr = argv[++i];
        } else if (arg == "--validate-only") {
            args.validateOnly = true;
        } else if (arg == "--help") {
            args.showHelp = true;
        }
    }
    return args;
}

void ApplyProjectConfig(EngineConfig& cfg, EngineMode mode) {
    cfg.mode = mode;
    if (project::ProjectManager::Get().IsLoaded()) {
        cfg.tickRate = project::ProjectManager::Get().Descriptor().runtime.tickRate;
        cfg.assetRoot = project::ProjectManager::Get().Descriptor().assets.root;
    }
}

module::GameModuleContext MakeModuleContext(
    Engine& engine,
    net::ReplicationManager& replication,
    asset::AssetRegistry& assets) {
    return {
        engine.GetWorld(),
        engine.GetNet(),
        replication,
        rules::ServerRules::Get(),
        assets,
        project::ProjectManager::Get().Descriptor()
    };
}

bool LoadAndStartModule(
    module::ModuleLoader& loader,
    const std::string& modulePath,
    Engine& engine,
    net::ReplicationManager& replication,
    asset::AssetRegistry& assets,
    bool configureServerRules) {

    auto result = loader.Load(modulePath);
    if (result != module::ModuleLoadResult::Success) {
        std::cerr << "Failed to load game module: " << modulePath << std::endl;
        return false;
    }

    auto ctx = MakeModuleContext(engine, replication, assets);
    auto* mod = loader.GetModule();
    mod->RegisterTypes(ctx);
    mod->ConfigureReplication(ctx);
    if (configureServerRules) {
        mod->ConfigureServerRules(ctx);
    }
    mod->OnStart(ctx);

    auto desc = mod->Describe();
    Logger::Info(std::string("Game module loaded: ") + desc.name);
    return true;
}

void ShutdownModule(
    module::ModuleLoader& loader,
    Engine& engine,
    net::ReplicationManager& replication,
    asset::AssetRegistry& assets) {
    if (loader.IsLoaded()) {
        auto ctx = MakeModuleContext(engine, replication, assets);
        loader.GetModule()->OnShutdown(ctx);
    }
}

} // namespace atlas::bootstrap
