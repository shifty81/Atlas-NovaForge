#include "../engine/core/Engine.h"
#include "../engine/module/IGameModule.h"
#include "../engine/net/NetContext.h"
#include "../engine/net/Replication.h"
#include "../engine/rules/ServerRules.h"
#include "../engine/assets/AssetRegistry.h"
#include "../engine/project/ProjectManager.h"
#include "../engine/physics/PhysicsWorld.h"
#include "../engine/flow/GameFlowGraph.h"
#include "../engine/flow/GameFlowNodes.h"
#include <iostream>
#include <cassert>

using namespace atlas;

// --- Test Module that tracks OnTick calls ---

class IntegrationTestModule : public module::IGameModule {
public:
    int tickCount = 0;
    float totalDt = 0.0f;
    bool startCalled = false;
    bool shutdownCalled = false;
    bool physicsAccessible = false;

    module::GameModuleDesc Describe() const override {
        return {"IntegrationTest", 1};
    }

    void RegisterTypes(module::GameModuleContext& ctx) override {
        (void)ctx;
    }

    void OnStart(module::GameModuleContext& ctx) override {
        startCalled = true;
        physicsAccessible = (ctx.physics != nullptr);
    }

    void OnTick(module::GameModuleContext& ctx, float dt) override {
        (void)ctx;
        ++tickCount;
        totalDt += dt;
    }

    void OnShutdown(module::GameModuleContext& ctx) override {
        (void)ctx;
        shutdownCalled = true;
    }
};

// --- Engine ticks the module via SetGameModule ---

void test_engine_module_tick_integration() {
    EngineConfig cfg;
    cfg.mode = EngineMode::Server;
    cfg.tickRate = 60;
    cfg.maxTicks = 5;

    Engine engine(cfg);
    engine.InitCore();
    engine.InitECS();
    engine.InitNetworking();
    engine.GetScheduler().SetFramePacing(false);

    IntegrationTestModule mod;
    atlas::net::NetContext dummyNet;
    atlas::net::ReplicationManager replication;
    atlas::asset::AssetRegistry assets;

    module::GameModuleContext ctx{
        engine.GetWorld(),
        engine.GetNet(),
        replication,
        atlas::rules::ServerRules::Get(),
        assets,
        atlas::project::ProjectManager::Get().Descriptor(),
        &engine.GetPhysics()
    };

    mod.OnStart(ctx);
    assert(mod.startCalled);
    assert(mod.physicsAccessible);

    engine.SetGameModule(&mod, &ctx);

    engine.Run();

    // Module should have been ticked 5 times (maxTicks = 5)
    assert(mod.tickCount == 5);
    assert(mod.totalDt > 0.0f);

    mod.OnShutdown(ctx);
    assert(mod.shutdownCalled);

    std::cout << "[PASS] test_engine_module_tick_integration" << std::endl;
}

// --- Physics is stepped during simulation ---

void test_engine_physics_integration() {
    EngineConfig cfg;
    cfg.mode = EngineMode::Server;
    cfg.tickRate = 60;
    cfg.maxTicks = 10;

    Engine engine(cfg);
    engine.InitCore();
    engine.InitECS();
    engine.InitNetworking();
    engine.GetScheduler().SetFramePacing(false);

    // Create a body with upward velocity to counteract gravity for a bit
    auto& physics = engine.GetPhysics();
    auto bodyId = physics.CreateBody(1.0f, false);
    physics.SetPosition(bodyId, 0.0f, 10.0f, 0.0f);
    physics.SetVelocity(bodyId, 0.0f, 0.0f, 0.0f);

    float initialY = physics.GetBody(bodyId)->position.y;

    engine.Run();

    // After 10 ticks with gravity, the body should have moved down
    float finalY = physics.GetBody(bodyId)->position.y;
    assert(finalY < initialY);

    std::cout << "[PASS] test_engine_physics_integration" << std::endl;
}

// --- Engine exposes PhysicsWorld ---

void test_engine_physics_accessible() {
    EngineConfig cfg;
    cfg.mode = EngineMode::Server;

    Engine engine(cfg);
    engine.InitCore();
    engine.InitECS();

    auto& physics = engine.GetPhysics();
    auto id = physics.CreateBody(2.0f, false);
    assert(physics.BodyCount() == 1);
    assert(physics.GetBody(id)->mass == 2.0f);

    std::cout << "[PASS] test_engine_physics_accessible" << std::endl;
}

// --- Flow graph is executed during simulation ---

void test_engine_flow_graph_integration() {
    EngineConfig cfg;
    cfg.mode = EngineMode::Server;
    cfg.tickRate = 60;
    cfg.maxTicks = 3;

    Engine engine(cfg);
    engine.InitCore();
    engine.InitECS();
    engine.InitNetworking();
    engine.GetScheduler().SetFramePacing(false);

    // Build a simple flow graph: a single StateNode (no edges needed)
    auto& flowGraph = engine.GetFlowGraph();
    auto stateId = flowGraph.AddNode(std::make_unique<flow::StateNode>());

    bool compiled = flowGraph.Compile();
    assert(compiled);

    engine.Run();

    // The flow graph should have been executed (state node produces output)
    auto* stateOutput = flowGraph.GetOutput(stateId, 0);
    assert(stateOutput != nullptr);
    assert(!stateOutput->data.empty());

    std::cout << "[PASS] test_engine_flow_graph_integration" << std::endl;
}

// --- Server mode also ticks physics and module ---

void test_engine_server_full_integration() {
    EngineConfig cfg;
    cfg.mode = EngineMode::Server;
    cfg.tickRate = 30;
    cfg.maxTicks = 5;

    Engine engine(cfg);
    engine.InitCore();
    engine.InitECS();
    engine.InitNetworking();
    engine.GetScheduler().SetFramePacing(false);

    // Set up physics body
    auto bodyId = engine.GetPhysics().CreateBody(1.0f, false);
    engine.GetPhysics().SetPosition(bodyId, 0.0f, 100.0f, 0.0f);

    // Set up module
    IntegrationTestModule mod;
    atlas::net::ReplicationManager replication;
    atlas::asset::AssetRegistry assets;

    module::GameModuleContext ctx{
        engine.GetWorld(),
        engine.GetNet(),
        replication,
        atlas::rules::ServerRules::Get(),
        assets,
        atlas::project::ProjectManager::Get().Descriptor(),
        &engine.GetPhysics()
    };

    mod.OnStart(ctx);
    engine.SetGameModule(&mod, &ctx);

    engine.Run();

    // Physics should have stepped
    float finalY = engine.GetPhysics().GetBody(bodyId)->position.y;
    assert(finalY < 100.0f);

    // Module should have been ticked
    assert(mod.tickCount == 5);

    // WorldState should have snapshots (server takes snapshots)
    assert(engine.GetWorldState().SnapshotCount() == 5);

    std::cout << "[PASS] test_engine_server_full_integration" << std::endl;
}

// --- Module context includes physics ---

void test_module_context_physics_access() {
    EngineConfig cfg;
    cfg.mode = EngineMode::Server;

    Engine engine(cfg);
    engine.InitCore();
    engine.InitECS();

    atlas::net::ReplicationManager replication;
    atlas::asset::AssetRegistry assets;

    module::GameModuleContext ctx{
        engine.GetWorld(),
        engine.GetNet(),
        replication,
        atlas::rules::ServerRules::Get(),
        assets,
        atlas::project::ProjectManager::Get().Descriptor(),
        &engine.GetPhysics()
    };

    assert(ctx.physics != nullptr);
    auto bodyId = ctx.physics->CreateBody(5.0f, true);
    assert(ctx.physics->GetBody(bodyId)->mass == 5.0f);
    assert(ctx.physics->GetBody(bodyId)->isStatic == true);

    std::cout << "[PASS] test_module_context_physics_access" << std::endl;
}
