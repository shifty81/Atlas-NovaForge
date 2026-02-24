#pragma once
#include "BehaviorGraph.h"

namespace atlas::ai {

// Reads AIContext.threatLevel and outputs a Perception value
class ThreatAssessmentNode : public BehaviorNode {
public:
    const char* GetName() const override { return "ThreatAssessment"; }
    const char* GetCategory() const override { return "Perception"; }
    std::vector<BehaviorPort> Inputs() const override { return {}; }
    std::vector<BehaviorPort> Outputs() const override;
    void Evaluate(const AIContext& ctx, const std::vector<BehaviorValue>& inputs, std::vector<BehaviorValue>& outputs) const override;
};

// Computes a weighted utility score from multiple Float inputs
class UtilityScoreNode : public BehaviorNode {
public:
    float threatWeight = 1.0f;
    float healthWeight = 1.0f;

    const char* GetName() const override { return "UtilityScore"; }
    const char* GetCategory() const override { return "Utility"; }
    std::vector<BehaviorPort> Inputs() const override;
    std::vector<BehaviorPort> Outputs() const override;
    void Evaluate(const AIContext& ctx, const std::vector<BehaviorValue>& inputs, std::vector<BehaviorValue>& outputs) const override;
};

// Selects the action with the highest utility score
class ActionSelectorNode : public BehaviorNode {
public:
    const char* GetName() const override { return "ActionSelector"; }
    const char* GetCategory() const override { return "Action"; }
    std::vector<BehaviorPort> Inputs() const override;
    std::vector<BehaviorPort> Outputs() const override;
    void Evaluate(const AIContext& ctx, const std::vector<BehaviorValue>& inputs, std::vector<BehaviorValue>& outputs) const override;
};

// Computes emotion state from AIContext threat and morale
class EmotionUpdateNode : public BehaviorNode {
public:
    const char* GetName() const override { return "EmotionUpdate"; }
    const char* GetCategory() const override { return "Emotion"; }
    std::vector<BehaviorPort> Inputs() const override { return {}; }
    std::vector<BehaviorPort> Outputs() const override;
    void Evaluate(const AIContext& ctx, const std::vector<BehaviorValue>& inputs, std::vector<BehaviorValue>& outputs) const override;
};

// Evaluates group tactical behavior: flanking, retreating, regrouping
// Outputs a group tactic index and effectiveness score
class GroupTacticsNode : public BehaviorNode {
public:
    float flankWeight = 1.0f;
    float retreatWeight = 1.0f;

    const char* GetName() const override { return "GroupTactics"; }
    const char* GetCategory() const override { return "Tactics"; }
    std::vector<BehaviorPort> Inputs() const override;
    std::vector<BehaviorPort> Outputs() const override;
    void Evaluate(const AIContext& ctx, const std::vector<BehaviorValue>& inputs, std::vector<BehaviorValue>& outputs) const override;
};

// Adapts NPC behavior difficulty based on player performance metrics
// Outputs an adjusted difficulty multiplier
class AdaptiveBehaviorNode : public BehaviorNode {
public:
    float adaptationRate = 0.1f;

    const char* GetName() const override { return "AdaptiveBehavior"; }
    const char* GetCategory() const override { return "Personalization"; }
    std::vector<BehaviorPort> Inputs() const override;
    std::vector<BehaviorPort> Outputs() const override;
    void Evaluate(const AIContext& ctx, const std::vector<BehaviorValue>& inputs, std::vector<BehaviorValue>& outputs) const override;
};

}
