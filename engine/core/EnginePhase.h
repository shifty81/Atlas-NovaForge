#pragma once
#include <cstdint>

namespace atlas {

/// Engine lifecycle phases.
/// The engine transitions through these phases during startup,
/// operation and shutdown.  Systems can query the current phase
/// to assert correct initialisation order and to adapt their
/// behaviour (e.g. the UI switches between launcher and editor
/// layouts depending on the phase).
enum class EnginePhase : uint8_t {
    Boot,          ///< Core systems initialising
    Launcher,      ///< Project browser / launcher screen
    ProjectLoad,   ///< Loading a selected project
    Editor,        ///< Full editor mode
    Client,        ///< Game client running
    Server,        ///< Dedicated server running
    Shutdown       ///< Cleanup in progress
};

/// Returns a human-readable name for the given phase.
inline const char* EnginePhaseToString(EnginePhase phase) {
    switch (phase) {
        case EnginePhase::Boot:        return "Boot";
        case EnginePhase::Launcher:    return "Launcher";
        case EnginePhase::ProjectLoad: return "ProjectLoad";
        case EnginePhase::Editor:      return "Editor";
        case EnginePhase::Client:      return "Client";
        case EnginePhase::Server:      return "Server";
        case EnginePhase::Shutdown:    return "Shutdown";
    }
    return "Unknown";
}

} // namespace atlas
