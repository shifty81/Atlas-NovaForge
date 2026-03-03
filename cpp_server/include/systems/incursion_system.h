#ifndef NOVAFORGE_SYSTEMS_INCURSION_SYSTEM_H
#define NOVAFORGE_SYSTEMS_INCURSION_SYSTEM_H

#include "ecs/system.h"
#include <string>

namespace atlas {
namespace systems {

/**
 * @brief Dynamic group PVE incursion event system
 *
 * Manages incursion lifecycle: wave spawning/defeat, influence tracking,
 * participant roster, reward pool, and state transitions from
 * Pending → Active → Withdrawing → Defeated.
 */
class IncursionSystem : public ecs::System {
public:
    explicit IncursionSystem(ecs::World* world);
    ~IncursionSystem() override = default;

    void update(float delta_time) override;
    std::string getName() const override { return "IncursionSystem"; }

    bool initialize(const std::string& entity_id, const std::string& incursion_id,
                    const std::string& system_id, int tier);
    bool addWave(const std::string& entity_id, int wave_number,
                 const std::string& npc_type, int count);
    bool spawnWave(const std::string& entity_id, int wave_number);
    bool defeatWave(const std::string& entity_id, int wave_number);
    bool joinIncursion(const std::string& entity_id, const std::string& participant_id);
    bool leaveIncursion(const std::string& entity_id, const std::string& participant_id);
    bool contributeRewards(const std::string& entity_id, float amount);
    float getInfluence(const std::string& entity_id) const;
    int getParticipantCount(const std::string& entity_id) const;
    int getState(const std::string& entity_id) const;
    float getRewardPool(const std::string& entity_id) const;
    int getTotalWavesDefeated(const std::string& entity_id) const;
};

} // namespace systems
} // namespace atlas

#endif // NOVAFORGE_SYSTEMS_INCURSION_SYSTEM_H
