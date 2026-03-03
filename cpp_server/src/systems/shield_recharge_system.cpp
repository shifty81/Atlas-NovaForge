#include "systems/shield_recharge_system.h"
#include "ecs/world.h"
#include <algorithm>

namespace atlas {
namespace systems {

void ShieldRechargeSystem::processEntity(ecs::Entity* /*entity*/,
                                          components::Health* health,
                                          float dt) {
    if (health->shield_hp < health->shield_max) {
        float recharge = health->shield_recharge_rate * dt;
        health->shield_hp = std::min(health->shield_hp + recharge, health->shield_max);
    }
}

float ShieldRechargeSystem::getShieldPercentage(const std::string& entity_id) const {
    auto* entity = world_->getEntity(entity_id);
    if (!entity) return -1.0f;
    
    auto* health = entity->getComponent<components::Health>();
    if (!health || health->shield_max <= 0.0f) return -1.0f;
    
    return health->shield_hp / health->shield_max;
}

} // namespace systems
} // namespace atlas
