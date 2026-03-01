// EVEOFFLINE ECS Adapter
// Bridges EVEOFFLINE game components and systems to the Atlas Engine ECS.

#pragma once

namespace eveoffline {
namespace adapters {

// Registers all EVEOFFLINE-specific ECS components with the engine world.
// Components include ship, fleet, market, mining, and faction state.
void RegisterComponents();

// Registers all EVEOFFLINE-specific ECS systems with the engine world.
// Systems include combat, movement, AI, economy, and background simulation.
void RegisterSystems();

}
}
