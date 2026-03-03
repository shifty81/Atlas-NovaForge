// EVEOFFLINE Game Initialization
// Entry point for the EVEOFFLINE Atlas project.
// Loaded by the Atlas Engine when the eveoffline project is activated.

#pragma once
#include <string>

namespace eveoffline {

struct GameConfig {
    std::string projectRoot;
    bool deterministicMode = true;
    bool serverAuthoritative = true;
    uint32_t tickRate = 30;
    uint32_t maxPlayers = 20;
};

// Registers EVEOFFLINE-specific ECS components, systems, and AI signals
// with the Atlas Engine. Called by the engine after project manifest is loaded.
bool Initialize(const GameConfig& config);

// Shuts down EVEOFFLINE-specific systems and releases resources.
void Shutdown();

// Returns the game version string from the project manifest.
const char* GetVersion();

}
