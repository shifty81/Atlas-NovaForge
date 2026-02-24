#pragma once
#include <cstdint>
#include <memory>

namespace atlas::ecs { class World; }
namespace atlas::net { class NetContext; class ReplicationManager; }
namespace atlas::rules { class ServerRules; }
namespace atlas::asset { class AssetRegistry; }
namespace atlas::project { struct ProjectDescriptor; }

namespace atlas::module {

struct GameModuleDesc {
    const char* name = "";
    uint32_t version = 0;
};

struct GameModuleContext {
    ecs::World& world;
    net::NetContext& net;
    net::ReplicationManager& replication;
    rules::ServerRules& rules;
    asset::AssetRegistry& assets;
    const project::ProjectDescriptor& project;
};

class IGameModule {
public:
    virtual GameModuleDesc Describe() const = 0;
    virtual void RegisterTypes(GameModuleContext& ctx) = 0;
    virtual void ConfigureReplication(GameModuleContext& ctx) { (void)ctx; }
    virtual void ConfigureServerRules(GameModuleContext& ctx) { (void)ctx; }
    virtual void OnStart(GameModuleContext& ctx) { (void)ctx; }
    virtual void OnTick(GameModuleContext& ctx, float dt) { (void)ctx; (void)dt; }
    virtual void OnShutdown(GameModuleContext& ctx) { (void)ctx; }
    virtual ~IGameModule() = default;
};

using CreateGameModuleFn = IGameModule*(*)();

} // namespace atlas::module

// Factory symbol exported by each game module (dll/so).
// Returns a raw pointer; the caller (ModuleLoader) takes ownership.
// extern "C" atlas::module::IGameModule* CreateGameModule();
