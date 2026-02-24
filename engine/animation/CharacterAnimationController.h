#pragma once
#include "AnimationGraph.h"
#include "AnimationNodes.h"
#include "../character/CharacterGraph.h"
#include <cstdint>
#include <string>

namespace atlas::animation {

/// Predefined animation states for a humanoid character.
enum class CharacterAnimState : uint8_t {
    Idle,
    Walk,
    Run,
    Attack,
    Hit,
    Death,
    Count
};

/// High-level animation controller that manages transitions between
/// predefined humanoid animation states.  Internally it builds and
/// drives an AnimationGraph so the animation pipeline remains
/// data-driven and deterministic.
class CharacterAnimationController {
public:
    CharacterAnimationController();

    /// Set the active animation state.  The controller will blend
    /// from the current state over `blendTime` seconds.
    void SetState(CharacterAnimState state, float blendTime = 0.2f);
    CharacterAnimState CurrentState() const { return m_state; }

    /// Advance the animation by `dt` seconds.
    void Update(float dt);

    /// Speed multiplier applied to all animations.
    void SetSpeed(float speed) { m_speed = speed; }
    float Speed() const { return m_speed; }

    /// Fatigue/damage modifiers forwarded to the animation graph.
    void SetFatigue(float f) { m_fatigue = f; }
    void SetDamageLevel(float d) { m_damage = d; }

    /// Returns the current skeletal pose (8 floats – 2 bones × 4).
    const std::vector<float>& CurrentPose() const { return m_currentPose; }

    /// Total accumulated time.
    float ElapsedTime() const { return m_elapsed; }

    /// Tick counter (incremented each Update call).
    uint32_t Tick() const { return m_tick; }

    /// Returns clip length for a given animation state.
    static float ClipLengthForState(CharacterAnimState state);

private:
    CharacterAnimState m_state = CharacterAnimState::Idle;
    float m_speed   = 1.0f;
    float m_fatigue = 0.0f;
    float m_damage  = 0.0f;
    float m_elapsed = 0.0f;
    uint32_t m_tick = 0;
    float m_blendTime = 0.2f;

    std::vector<float> m_currentPose;

    AnimationGraph m_graph;
    AnimNodeID m_clipNode  = 0;
    AnimNodeID m_blendNode = 0;
    AnimNodeID m_modNode   = 0;

    void RebuildGraph();
};

} // namespace atlas::animation
