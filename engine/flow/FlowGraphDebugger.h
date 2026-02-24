#pragma once
#include "GameFlowGraph.h"
#include <set>
#include <vector>
#include <functional>

namespace atlas::flow {

enum class DebugAction : uint8_t {
    Continue,
    StepNode,    // Execute one node
    StepFrame,   // Execute one full frame
    Pause
};

struct DebugNodeState {
    FlowNodeID nodeID = 0;
    std::vector<FlowValue> inputs;
    std::vector<FlowValue> outputs;
    bool wasExecuted = false;
};

class FlowGraphDebugger {
public:
    // Breakpoints
    void AddBreakpoint(FlowNodeID nodeID);
    void RemoveBreakpoint(FlowNodeID nodeID);
    bool HasBreakpoint(FlowNodeID nodeID) const;
    void ClearBreakpoints();
    const std::set<FlowNodeID>& Breakpoints() const;

    // Execution control
    void SetAction(DebugAction action);
    DebugAction CurrentAction() const;
    bool IsPaused() const;

    // Execution with debugging
    bool ExecuteDebug(GameFlowGraph& graph, const FlowContext& ctx);

    // Value inspection
    const std::vector<DebugNodeState>& NodeStates() const;
    const DebugNodeState* GetNodeState(FlowNodeID nodeID) const;

    // Timeline
    size_t ExecutionStep() const;
    FlowNodeID CurrentNode() const;

    // Callback for breakpoint hits
    using BreakpointCallback = std::function<void(FlowNodeID, const DebugNodeState&)>;
    void SetBreakpointCallback(BreakpointCallback cb);

private:
    std::set<FlowNodeID> m_breakpoints;
    DebugAction m_action = DebugAction::Continue;
    std::vector<DebugNodeState> m_nodeStates;
    size_t m_executionStep = 0;
    FlowNodeID m_currentNode = 0;
    bool m_paused = false;
    BreakpointCallback m_callback;
};

} // namespace atlas::flow
