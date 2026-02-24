#include "BehaviorNodes.h"
#include <algorithm>

namespace atlas::ai {

// --- ThreatAssessmentNode ---

std::vector<BehaviorPort> ThreatAssessmentNode::Outputs() const {
    return {{"Threat", BehaviorPinType::Perception}};
}

void ThreatAssessmentNode::Evaluate(const AIContext& ctx, const std::vector<BehaviorValue>& /*inputs*/, std::vector<BehaviorValue>& outputs) const {
    outputs.resize(1);
    outputs[0].type = BehaviorPinType::Perception;
    outputs[0].data = {ctx.threatLevel};
}

// --- UtilityScoreNode ---

std::vector<BehaviorPort> UtilityScoreNode::Inputs() const {
    return {
        {"ThreatScore", BehaviorPinType::Float},
        {"HealthScore", BehaviorPinType::Float}
    };
}

std::vector<BehaviorPort> UtilityScoreNode::Outputs() const {
    return {{"Score", BehaviorPinType::Float}};
}

void UtilityScoreNode::Evaluate(const AIContext& /*ctx*/, const std::vector<BehaviorValue>& inputs, std::vector<BehaviorValue>& outputs) const {
    float score = 0.0f;

    if (!inputs.empty() && !inputs[0].data.empty()) {
        score += inputs[0].data[0] * threatWeight;
    }
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        score += inputs[1].data[0] * healthWeight;
    }

    outputs.resize(1);
    outputs[0].type = BehaviorPinType::Float;
    outputs[0].data = {score};
}

// --- ActionSelectorNode ---

std::vector<BehaviorPort> ActionSelectorNode::Inputs() const {
    return {
        {"UtilityA", BehaviorPinType::Float},
        {"UtilityB", BehaviorPinType::Float}
    };
}

std::vector<BehaviorPort> ActionSelectorNode::Outputs() const {
    return {{"SelectedAction", BehaviorPinType::Action}};
}

void ActionSelectorNode::Evaluate(const AIContext& /*ctx*/, const std::vector<BehaviorValue>& inputs, std::vector<BehaviorValue>& outputs) const {
    float scoreA = 0.0f;
    float scoreB = 0.0f;

    if (!inputs.empty() && !inputs[0].data.empty()) {
        scoreA = inputs[0].data[0];
    }
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        scoreB = inputs[1].data[0];
    }

    // Output: index of winning action (0 or 1) and its score
    float selected = (scoreA >= scoreB) ? 0.0f : 1.0f;

    outputs.resize(1);
    outputs[0].type = BehaviorPinType::Action;
    outputs[0].data = {selected, std::max(scoreA, scoreB)};
}

// --- EmotionUpdateNode ---

std::vector<BehaviorPort> EmotionUpdateNode::Outputs() const {
    return {{"Emotions", BehaviorPinType::EmotionState}};
}

void EmotionUpdateNode::Evaluate(const AIContext& ctx, const std::vector<BehaviorValue>& /*inputs*/, std::vector<BehaviorValue>& outputs) const {
    // Emotion vector: [fear, confidence, anger]
    float fear = ctx.threatLevel * (1.0f - ctx.morale);
    float confidence = ctx.morale * ctx.healthPercent;
    float anger = ctx.threatLevel * ctx.morale;

    outputs.resize(1);
    outputs[0].type = BehaviorPinType::EmotionState;
    outputs[0].data = {fear, confidence, anger};
}

// --- GroupTacticsNode ---

std::vector<BehaviorPort> GroupTacticsNode::Inputs() const {
    return {
        {"AllyCount", BehaviorPinType::Float},
        {"EnemyCount", BehaviorPinType::Float}
    };
}

std::vector<BehaviorPort> GroupTacticsNode::Outputs() const {
    return {{"Tactic", BehaviorPinType::Action}};
}

void GroupTacticsNode::Evaluate(const AIContext& ctx, const std::vector<BehaviorValue>& inputs, std::vector<BehaviorValue>& outputs) const {
    float allies = 1.0f;
    float enemies = 1.0f;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        allies = inputs[0].data[0];
    }
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        enemies = inputs[1].data[0];
    }

    // Compute tactical scores
    constexpr float kDefaultAdvantageWhenNoEnemies = 2.0f;
    float numericAdvantage = (enemies > 0.0f) ? allies / enemies : kDefaultAdvantageWhenNoEnemies;

    // Tactic 0=Charge, 1=Flank, 2=Hold, 3=Retreat
    float flankScore = numericAdvantage * flankWeight * ctx.morale;
    float retreatScore = (1.0f - ctx.healthPercent) * retreatWeight * ctx.threatLevel;
    float chargeScore = numericAdvantage * ctx.morale * ctx.healthPercent;
    float holdScore = (1.0f - ctx.threatLevel) * ctx.healthPercent;

    // Select best tactic
    float bestScore = chargeScore;
    float tactic = 0.0f;

    if (flankScore > bestScore) {
        bestScore = flankScore;
        tactic = 1.0f;
    }
    if (holdScore > bestScore) {
        bestScore = holdScore;
        tactic = 2.0f;
    }
    if (retreatScore > bestScore) {
        bestScore = retreatScore;
        tactic = 3.0f;
    }

    outputs.resize(1);
    outputs[0].type = BehaviorPinType::Action;
    outputs[0].data = {tactic, bestScore};
}

// --- AdaptiveBehaviorNode ---

std::vector<BehaviorPort> AdaptiveBehaviorNode::Inputs() const {
    return {
        {"PlayerWinRate", BehaviorPinType::Float},
        {"PlayerDeathRate", BehaviorPinType::Float}
    };
}

std::vector<BehaviorPort> AdaptiveBehaviorNode::Outputs() const {
    return {{"DifficultyMultiplier", BehaviorPinType::Float}};
}

void AdaptiveBehaviorNode::Evaluate(const AIContext& /*ctx*/, const std::vector<BehaviorValue>& inputs, std::vector<BehaviorValue>& outputs) const {
    float winRate = 0.5f;
    float deathRate = 0.5f;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        winRate = inputs[0].data[0];
    }
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        deathRate = inputs[1].data[0];
    }

    // Adaptive difficulty: if player wins too much, increase difficulty
    // If player dies too much, decrease difficulty
    // Target win rate is ~0.5 for balanced gameplay
    float winDelta = winRate - 0.5f;
    float deathDelta = deathRate - 0.5f;
    float adjustment = (winDelta - deathDelta) * adaptationRate;

    // Clamp difficulty multiplier to [0.5, 2.0]
    float multiplier = 1.0f + adjustment;
    multiplier = std::max(0.5f, std::min(2.0f, multiplier));

    outputs.resize(1);
    outputs[0].type = BehaviorPinType::Float;
    outputs[0].data = {multiplier};
}

}
