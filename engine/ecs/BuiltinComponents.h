#pragma once
#include "ComponentCategory.h"
#include <cstdint>
#include <string>
#include <vector>

namespace atlas::ecs {

/// Transform: position, rotation, scale — core simulated state.
struct TransformComponent {
    float x = 0, y = 0, z = 0;
    float rx = 0, ry = 0, rz = 0;
    float sx = 1, sy = 1, sz = 1;
};

/// Velocity: physics simulated state.
struct VelocityComponent {
    float vx = 0, vy = 0, vz = 0;
};

/// Health: gameplay simulated state.
struct HealthComponent {
    float current = 100;
    float max = 100;
};

/// Name tag: simulated metadata.
struct NameComponent {
    std::string name;
};

/// Render hint: presentation-only, not serialized or hashed.
struct RenderHintComponent {
    uint32_t meshId = 0;
    uint32_t materialId = 0;
    float lodBias = 1.0f;
};

/// Debug label: debug overlays, stripped in release.
struct DebugLabelComponent {
    std::string label;
    uint32_t color = 0xFFFFFF;
};

/// Path cache: derived state, recomputed each tick.
struct PathCacheComponent {
    std::vector<float> waypoints;
    bool valid = false;
};

// --- Category annotations ---

template<>
struct ComponentCategoryTrait<TransformComponent> {
    static constexpr sim::StateCategory category = sim::StateCategory::Simulated;
};

template<>
struct ComponentCategoryTrait<VelocityComponent> {
    static constexpr sim::StateCategory category = sim::StateCategory::Simulated;
};

template<>
struct ComponentCategoryTrait<HealthComponent> {
    static constexpr sim::StateCategory category = sim::StateCategory::Simulated;
};

template<>
struct ComponentCategoryTrait<NameComponent> {
    static constexpr sim::StateCategory category = sim::StateCategory::Simulated;
};

template<>
struct ComponentCategoryTrait<RenderHintComponent> {
    static constexpr sim::StateCategory category = sim::StateCategory::Presentation;
};

template<>
struct ComponentCategoryTrait<DebugLabelComponent> {
    static constexpr sim::StateCategory category = sim::StateCategory::Debug;
};

template<>
struct ComponentCategoryTrait<PathCacheComponent> {
    static constexpr sim::StateCategory category = sim::StateCategory::Derived;
};

}  // namespace atlas::ecs
