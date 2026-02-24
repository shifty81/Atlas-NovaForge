#pragma once
#include "../sim/WorldState.h"
#include <type_traits>

namespace atlas::ecs {

/// Type trait for associating an ECS component with a StateCategory.
/// Specialize this for each component type:
///
///   template<>
///   struct ComponentCategoryTrait<TransformComponent> {
///       static constexpr sim::StateCategory category = sim::StateCategory::Simulated;
///   };
///
/// Unspecialized components default to Simulated.
template <typename T, typename = void>
struct ComponentCategoryTrait {
    static constexpr sim::StateCategory category = sim::StateCategory::Simulated;
};

/// Convenience accessor.
template <typename T>
constexpr sim::StateCategory component_category_v = ComponentCategoryTrait<T>::category;

/// Compile-time check: true if a component has an explicit category specialization.
template <typename T, typename = void>
struct HasComponentCategory : std::false_type {};

template <typename T>
struct HasComponentCategory<T, std::void_t<decltype(ComponentCategoryTrait<T>::category)>> : std::true_type {};

template <typename T>
constexpr bool has_component_category_v = HasComponentCategory<T>::value;

/// Compile-time assertions for enforcing category rules.
/// Use these in systems that should only access certain state categories.

/// Assert that T is a Simulated component (safe for simulation code).
template <typename T>
constexpr void assert_simulated() {
    static_assert(component_category_v<T> == sim::StateCategory::Simulated,
        "Only Simulated components may be used in simulation code");
}

/// Assert that T is NOT a Presentation component (safe for server code).
template <typename T>
constexpr void assert_not_presentation() {
    static_assert(component_category_v<T> != sim::StateCategory::Presentation,
        "Presentation components cannot be used in server/simulation code");
}

/// Helper macro for declaring component category in one line.
#define ATLAS_COMPONENT_CATEGORY(Type, Cat)                          \
    template<>                                                       \
    struct atlas::ecs::ComponentCategoryTrait<Type> {                 \
        static constexpr atlas::sim::StateCategory category =        \
            atlas::sim::StateCategory::Cat;                          \
    }

}  // namespace atlas::ecs
