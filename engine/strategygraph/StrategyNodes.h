#pragma once
#include "StrategyGraph.h"
#include "../ai/AISignalRegistry.h"
#include <cmath>

namespace atlas::strategy {

// InfluenceRing: Produces a scalar field representing control or pressure around a point
class InfluenceRingNode : public StrategyNode {
public:
    float radius = 5000.0f;
    float strength = 1.0f;
    float falloff = 1.0f;

    const char* GetName() const override { return "InfluenceRing"; }
    const char* GetCategory() const override { return "Strategy"; }

    std::vector<StrategyPort> Inputs() const override {
        return {};
    }

    std::vector<StrategyPort> Outputs() const override {
        return {{ "influence", StrategyPinType::ScalarField }};
    }

    void Evaluate(
        const StrategyContext& /*ctx*/,
        const std::vector<StrategyValue>& /*inputs*/,
        std::vector<StrategyValue>& outputs
    ) const override {
        StrategyValue val;
        val.type = StrategyPinType::ScalarField;
        val.data = { radius, strength, falloff };
        outputs[0] = std::move(val);
    }
};

// ThreatFieldNode: Aggregates hostile influence into a threat scalar
class ThreatFieldNode : public StrategyNode {
public:
    float hostilityWeight = 1.0f;

    const char* GetName() const override { return "ThreatField"; }
    const char* GetCategory() const override { return "Strategy"; }

    std::vector<StrategyPort> Inputs() const override {
        return {{ "influence", StrategyPinType::ScalarField }};
    }

    std::vector<StrategyPort> Outputs() const override {
        return {{ "threat", StrategyPinType::ScalarField }};
    }

    void Evaluate(
        const StrategyContext& /*ctx*/,
        const std::vector<StrategyValue>& inputs,
        std::vector<StrategyValue>& outputs
    ) const override {
        StrategyValue val;
        val.type = StrategyPinType::ScalarField;
        if (!inputs.empty() && !inputs[0].data.empty()) {
            float strength = inputs[0].data.size() > 1 ? inputs[0].data[1] : 0.0f;
            val.data = { strength * hostilityWeight };
        } else {
            val.data = { 0.0f };
        }
        outputs[0] = std::move(val);
    }
};

// ObjectiveScoreNode: Weighted sum of strategy inputs producing a single score
class ObjectiveScoreNode : public StrategyNode {
public:
    float controlWeight = 1.0f;
    float threatWeight = -0.5f;

    const char* GetName() const override { return "ObjectiveScore"; }
    const char* GetCategory() const override { return "Strategy"; }

    std::vector<StrategyPort> Inputs() const override {
        return {
            { "control", StrategyPinType::ScalarField },
            { "threat",  StrategyPinType::ScalarField }
        };
    }

    std::vector<StrategyPort> Outputs() const override {
        return {{ "score", StrategyPinType::ScalarField }};
    }

    void Evaluate(
        const StrategyContext& /*ctx*/,
        const std::vector<StrategyValue>& inputs,
        std::vector<StrategyValue>& outputs
    ) const override {
        float controlVal = 0.0f;
        float threatVal = 0.0f;

        if (inputs.size() > 0 && !inputs[0].data.empty()) {
            controlVal = inputs[0].data[0];
        }
        if (inputs.size() > 1 && !inputs[1].data.empty()) {
            threatVal = inputs[1].data[0];
        }

        float score = controlWeight * controlVal + threatWeight * threatVal;

        StrategyValue val;
        val.type = StrategyPinType::ScalarField;
        val.data = { score };
        outputs[0] = std::move(val);
    }
};

// ConstantNode: Produces a constant scalar field value
class StrategyConstantNode : public StrategyNode {
public:
    float value = 1.0f;

    const char* GetName() const override { return "Constant"; }
    const char* GetCategory() const override { return "Strategy"; }

    std::vector<StrategyPort> Inputs() const override {
        return {};
    }

    std::vector<StrategyPort> Outputs() const override {
        return {{ "value", StrategyPinType::ScalarField }};
    }

    void Evaluate(
        const StrategyContext& /*ctx*/,
        const std::vector<StrategyValue>& /*inputs*/,
        std::vector<StrategyValue>& outputs
    ) const override {
        StrategyValue val;
        val.type = StrategyPinType::ScalarField;
        val.data = { value };
        outputs[0] = std::move(val);
    }
};

// ReadAISignalNode: Reads a signal from AISignalRegistry and outputs its value
class ReadAISignalNode : public StrategyNode {
public:
    std::string signalName; // Qualified name, e.g. "faction.morale"

    const char* GetName() const override { return "ReadAISignal"; }
    const char* GetCategory() const override { return "AI"; }

    std::vector<StrategyPort> Inputs() const override {
        return {};
    }

    std::vector<StrategyPort> Outputs() const override {
        return {{ "signal", StrategyPinType::ScalarField }};
    }

    void Evaluate(
        const StrategyContext& /*ctx*/,
        const std::vector<StrategyValue>& /*inputs*/,
        std::vector<StrategyValue>& outputs
    ) const override {
        float val = atlas::ai::AISignalRegistry::Get().Read(signalName);
        StrategyValue out;
        out.type = StrategyPinType::ScalarField;
        out.data = { val };
        outputs[0] = std::move(out);
    }
};

// EmitActionNode: Produces an action recommendation with name and priority
class EmitActionNode : public StrategyNode {
public:
    std::string actionName;
    float basePriority = 1.0f;

    const char* GetName() const override { return "EmitAction"; }
    const char* GetCategory() const override { return "Strategy"; }

    std::vector<StrategyPort> Inputs() const override {
        return {{ "score", StrategyPinType::ScalarField }};
    }

    std::vector<StrategyPort> Outputs() const override {
        return {{ "priority", StrategyPinType::ScalarField }};
    }

    void Evaluate(
        const StrategyContext& /*ctx*/,
        const std::vector<StrategyValue>& inputs,
        std::vector<StrategyValue>& outputs
    ) const override {
        float inputScore = 0.0f;
        if (!inputs.empty() && !inputs[0].data.empty()) {
            inputScore = inputs[0].data[0];
        }
        float priority = basePriority * inputScore;

        StrategyValue out;
        out.type = StrategyPinType::ScalarField;
        out.data = { priority };
        outputs[0] = std::move(out);
    }
};

}
