#pragma once
#include "Engine.h"

namespace atlas {

enum class RuntimeMode {
    Editor,
    Game,
    Server
};

class RuntimeBootstrap {
public:
    bool IsInitialized() const { return m_engine != nullptr; }

    RuntimeMode Mode() const { return m_mode; }

    Engine& Initialize(RuntimeMode mode) {
        if (m_engine) return *m_engine;
        m_mode = mode;

        EngineConfig cfg;
        switch (mode) {
            case RuntimeMode::Editor:
                cfg.mode = EngineMode::Editor;
                cfg.headless = false;
                break;
            case RuntimeMode::Game:
                cfg.mode = EngineMode::Client;
                cfg.headless = false;
                break;
            case RuntimeMode::Server:
                cfg.mode = EngineMode::Server;
                cfg.headless = true;
                break;
        }

        m_engine = std::make_unique<Engine>(cfg);
        m_engine->InitCore();
        m_engine->InitNetworking();
        return *m_engine;
    }

private:
    RuntimeMode m_mode = RuntimeMode::Editor;
    std::unique_ptr<Engine> m_engine;
};

} // namespace atlas
