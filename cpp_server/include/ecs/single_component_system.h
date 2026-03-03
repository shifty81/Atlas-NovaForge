#ifndef NOVAFORGE_ECS_SINGLE_COMPONENT_SYSTEM_H
#define NOVAFORGE_ECS_SINGLE_COMPONENT_SYSTEM_H

#include "system.h"
#include "world.h"
#include <string>

namespace atlas {
namespace ecs {

/**
 * @brief Template base for systems that iterate over a single component type.
 *
 * Eliminates the repeated boilerplate of:
 *   1. Querying world_ for entities with component C.
 *   2. Null-checking the component pointer.
 *   3. Calling per-entity logic.
 *
 * Subclasses only need to implement processEntity() with their unique logic.
 *
 * Example:
 * @code
 *   class CapacitorSystem : public SingleComponentSystem<components::Capacitor> {
 *   public:
 *       using SingleComponentSystem::SingleComponentSystem;
 *       std::string getName() const override { return "CapacitorSystem"; }
 *   protected:
 *       void processEntity(Entity* entity, components::Capacitor* cap,
 *                          float dt) override {
 *           if (cap->capacitor < cap->capacitor_max) {
 *               float recharge = cap->recharge_rate * dt;
 *               cap->capacitor = std::min(cap->capacitor + recharge,
 *                                          cap->capacitor_max);
 *           }
 *       }
 *   };
 * @endcode
 */
template <typename C>
class SingleComponentSystem : public System {
public:
    using System::System;  // inherit constructor

    void update(float delta_time) override {
        auto entities = world_->getEntities<C>();
        for (auto* entity : entities) {
            auto* comp = entity->template getComponent<C>();
            if (!comp) continue;
            processEntity(entity, comp, delta_time);
        }
    }

protected:
    /**
     * @brief Called for each entity that has component C.
     * @param entity  The entity (never null).
     * @param comp    The component (never null).
     * @param dt      Delta time in seconds.
     */
    virtual void processEntity(Entity* entity, C* comp, float dt) = 0;
};

} // namespace ecs
} // namespace atlas

#endif // NOVAFORGE_ECS_SINGLE_COMPONENT_SYSTEM_H
