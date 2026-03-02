#ifndef NOVAFORGE_SYSTEMS_GALACTIC_RESPONSE_CURVE_SYSTEM_H
#define NOVAFORGE_SYSTEMS_GALACTIC_RESPONSE_CURVE_SYSTEM_H

#include "ecs/system.h"
#include <string>

namespace atlas {
namespace systems {

/**
 * @brief AI faction response escalation to galactic threats
 *
 * Manages threat level tracking, faction response curves, empire
 * reinforcement dispatching, and trade rerouting. When threats exceed
 * thresholds, factions escalate through response tiers.
 */
class GalacticResponseCurveSystem : public ecs::System {
public:
    explicit GalacticResponseCurveSystem(ecs::World* world);
    ~GalacticResponseCurveSystem() override = default;

    void update(float delta_time) override;
    std::string getName() const override { return "GalacticResponseCurveSystem"; }

    bool initializeFaction(const std::string& entity_id, const std::string& faction_id);
    bool reportThreat(const std::string& entity_id, float magnitude);
    bool dispatchReinforcement(const std::string& entity_id);
    bool rerouteTradeFor(const std::string& entity_id, const std::string& system_id);
    float getThreatLevel(const std::string& entity_id) const;
    int getResponseTier(const std::string& entity_id) const;
    int getReinforcementCount(const std::string& entity_id) const;
    int getReroutedSystemCount(const std::string& entity_id) const;
    float getEscalationRate(const std::string& entity_id) const;
    bool isFullMobilization(const std::string& entity_id) const;
};

} // namespace systems
} // namespace atlas

#endif // NOVAFORGE_SYSTEMS_GALACTIC_RESPONSE_CURVE_SYSTEM_H
