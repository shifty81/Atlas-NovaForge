// EVEOFFLINE ECS Adapter
// Bridges EVEOFFLINE game components and systems to the Atlas Engine ECS.

#include "ECSAdapter.h"

namespace eveoffline {
namespace adapters {

void RegisterComponents() {
    // Ship components: Hull, Fitting, Capacitor, Navigation
    // Fleet components: FleetMembership, FleetMorale, FleetFormation
    // Economy components: Wallet, MarketOrder, Manufacturing
    // Mining components: MiningLaser, MineralDeposit
    // Faction components: Standing, FactionMembership
    // Background sim: SectorTension, TradeFlow, PirateDoctrine
}

void RegisterSystems() {
    // Core: MovementSystem, CombatSystem, CapacitorSystem
    // AI: AISystem, PatrolSystem, MiningAISystem
    // Economy: MarketSystem, ManufacturingSystem, ContractSystem
    // Social: CorporationSystem, ChatSystem, StandingsSystem
    // Background: BackgroundSimSystem, SectorTensionSystem, TradeFlowSystem
}

}
}
