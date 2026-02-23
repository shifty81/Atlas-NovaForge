#include "systems/warp_auto_comfort_system.h"
#include "components/game_components.h"
#include "ecs/world.h"
#include <algorithm>
#include <cmath>

namespace atlas {
namespace systems {

WarpAutoComfortSystem::WarpAutoComfortSystem(ecs::World* world)
    : ecs::System(world) {}

float WarpAutoComfortSystem::computeComfortReduction(float current_fps, float target_fps,
                                                      float current_reduction, float delta_time) {
    if (target_fps <= 0.0f) return 0.0f;

    float low_threshold  = target_fps * 0.8f;   // Below this → increase reduction
    float high_threshold = target_fps * 0.95f;   // Above this → decrease reduction

    float ramp_up_speed   = 0.5f;   // Reduction increases at this rate/sec
    float ramp_down_speed = 0.3f;   // Reduction decreases at this rate/sec

    float result = current_reduction;

    if (current_fps < low_threshold) {
        // Performance is suffering — increase comfort reduction
        result += ramp_up_speed * delta_time;
    } else if (current_fps >= high_threshold) {
        // Performance is fine — decrease comfort reduction
        result -= ramp_down_speed * delta_time;
    }
    // Between thresholds: hold current value (hysteresis)

    return std::clamp(result, 0.0f, 1.0f);
}

void WarpAutoComfortSystem::applyComfort(float comfort_reduction, bool ultrawide,
                                          float max_distortion_uw,
                                          float& motion, float& blur) {
    // Reduce effects proportionally to comfort_reduction
    float scale = 1.0f - comfort_reduction * 0.6f;   // At max reduction, 40% of original
    motion *= scale;
    blur   *= scale;

    // Ultrawide clamp: limit blur (which drives distortion) to a maximum
    if (ultrawide) {
        blur = std::min(blur, max_distortion_uw);
    }

    // Ensure non-negative
    motion = std::max(motion, 0.0f);
    blur   = std::max(blur,   0.0f);
}

void WarpAutoComfortSystem::update(float delta_time) {
    if (!world_) return;

    for (auto* entity : world_->getEntities()) {
        auto* comfort = entity->getComponent<components::WarpAutoComfort>();
        auto* access  = entity->getComponent<components::WarpAccessibility>();
        if (!comfort || !access) continue;

        // Update comfort reduction based on FPS
        comfort->comfort_reduction = computeComfortReduction(
            comfort->current_fps, comfort->target_fps,
            comfort->comfort_reduction, delta_time);

        // Apply comfort adjustments to accessibility
        applyComfort(comfort->comfort_reduction, comfort->ultrawide_detected,
                     comfort->max_distortion_ultrawide,
                     access->motion_intensity, access->blur_intensity);
    }
}

} // namespace systems
} // namespace atlas
