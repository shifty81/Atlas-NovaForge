#include "GameFlowNodes.h"

namespace atlas::flow {

// --- StateNode ---

std::vector<FlowPort> StateNode::Inputs() const {
    return {{"Trigger", FlowPinType::Trigger}};
}

std::vector<FlowPort> StateNode::Outputs() const {
    return {{"State", FlowPinType::State}};
}

void StateNode::Evaluate(const FlowContext& /*ctx*/, const std::vector<FlowValue>& inputs, std::vector<FlowValue>& outputs) const {
    outputs.resize(1);
    outputs[0].type = FlowPinType::State;

    float active = 1.0f;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        active = inputs[0].data[0];
    }

    outputs[0].data = {active};
    outputs[0].text = stateName;
}

// --- TransitionNode ---

std::vector<FlowPort> TransitionNode::Inputs() const {
    return {
        {"State", FlowPinType::State},
        {"Condition", FlowPinType::Bool}
    };
}

std::vector<FlowPort> TransitionNode::Outputs() const {
    return {{"Trigger", FlowPinType::Trigger}};
}

void TransitionNode::Evaluate(const FlowContext& /*ctx*/, const std::vector<FlowValue>& inputs, std::vector<FlowValue>& outputs) const {
    outputs.resize(1);
    outputs[0].type = FlowPinType::Trigger;

    bool stateActive = false;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        stateActive = inputs[0].data[0] > 0.5f;
    }

    bool conditionMet = false;
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        conditionMet = inputs[1].data[0] > 0.5f;
    }

    float trigger = (stateActive && conditionMet) ? 1.0f : 0.0f;
    outputs[0].data = {trigger};
}

// --- TimerNode ---

std::vector<FlowPort> TimerNode::Inputs() const {
    return {
        {"Trigger", FlowPinType::Trigger},
        {"Duration", FlowPinType::Float}
    };
}

std::vector<FlowPort> TimerNode::Outputs() const {
    return {{"Trigger", FlowPinType::Trigger}};
}

void TimerNode::Evaluate(const FlowContext& ctx, const std::vector<FlowValue>& inputs, std::vector<FlowValue>& outputs) const {
    outputs.resize(1);
    outputs[0].type = FlowPinType::Trigger;

    float duration = 1.0f;
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        duration = inputs[1].data[0];
    }

    bool triggered = false;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        triggered = inputs[0].data[0] > 0.5f;
    }

    // Fire output trigger if input is triggered and elapsed time >= duration
    float fire = (triggered && ctx.elapsedTime >= duration) ? 1.0f : 0.0f;
    outputs[0].data = {fire};
}

// --- ConditionNode ---

std::vector<FlowPort> ConditionNode::Inputs() const {
    return {
        {"Value", FlowPinType::Float},
        {"Threshold", FlowPinType::Float}
    };
}

std::vector<FlowPort> ConditionNode::Outputs() const {
    return {{"Result", FlowPinType::Bool}};
}

void ConditionNode::Evaluate(const FlowContext& /*ctx*/, const std::vector<FlowValue>& inputs, std::vector<FlowValue>& outputs) const {
    float value = 0.0f;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        value = inputs[0].data[0];
    }

    float threshold = 0.5f;
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        threshold = inputs[1].data[0];
    }

    float result = (value >= threshold) ? 1.0f : 0.0f;

    outputs.resize(1);
    outputs[0].type = FlowPinType::Bool;
    outputs[0].data = {result};
}

}
