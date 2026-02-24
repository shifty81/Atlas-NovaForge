#pragma once
#include "GameFlowGraph.h"

namespace atlas::flow {

class StateNode : public FlowNode {
public:
    std::string stateName = "MainMenu";

    const char* GetName() const override { return "State"; }
    const char* GetCategory() const override { return "Flow"; }
    std::vector<FlowPort> Inputs() const override;
    std::vector<FlowPort> Outputs() const override;
    void Evaluate(const FlowContext& ctx, const std::vector<FlowValue>& inputs, std::vector<FlowValue>& outputs) const override;
};

class TransitionNode : public FlowNode {
public:
    const char* GetName() const override { return "Transition"; }
    const char* GetCategory() const override { return "Flow"; }
    std::vector<FlowPort> Inputs() const override;
    std::vector<FlowPort> Outputs() const override;
    void Evaluate(const FlowContext& ctx, const std::vector<FlowValue>& inputs, std::vector<FlowValue>& outputs) const override;
};

class TimerNode : public FlowNode {
public:
    const char* GetName() const override { return "Timer"; }
    const char* GetCategory() const override { return "Control"; }
    std::vector<FlowPort> Inputs() const override;
    std::vector<FlowPort> Outputs() const override;
    void Evaluate(const FlowContext& ctx, const std::vector<FlowValue>& inputs, std::vector<FlowValue>& outputs) const override;
};

class ConditionNode : public FlowNode {
public:
    const char* GetName() const override { return "Condition"; }
    const char* GetCategory() const override { return "Logic"; }
    std::vector<FlowPort> Inputs() const override;
    std::vector<FlowPort> Outputs() const override;
    void Evaluate(const FlowContext& ctx, const std::vector<FlowValue>& inputs, std::vector<FlowValue>& outputs) const override;
};

}
